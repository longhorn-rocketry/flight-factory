#include <unistd.h>

#include "ff_arduino_harness.hpp"
#include "flight_factory.hpp"

#define TELEM(data) outln(std::string("[#b$bffcore#r] ") + data)

namespace ff {

/**
 * Metric units are used for all flight statistics so as to conform with
 * Aimbot's Standard Atmosphere.
 */
static const std::string gUNIT_DISPLACEMENT = "m";
static const std::string gUNIT_TIME = "s";
static const std::string gUNIT_VELOCITY = gUNIT_DISPLACEMENT + "/" + gUNIT_TIME;
static const std::string gUNIT_ACCEL = gUNIT_VELOCITY + "^2";

static const std::string gFC_CONFIG_NAME = ".ff";
static const float gMETERS_TO_FEET = 3.28084;
static const float gBOOTUP_DURATION = 1.0;
static bool g_ff_initialized = false;

Simulator* g_ff_simulator = nullptr;
FlightFactoryConfiguration g_ff_config;

std::string g_ff_fc_path;

static const std::vector<std::string> gBOOTUP_ASCII = {
  "           /o-             .MMNmNM",
  "    `.    hMMd+`           .MMMhmM",
  "`.+ymN/   +mMMMNo`         -My/mMM",
  "+MMNyNMo-+hsoMMMMm/        +Ms-hMM",
  " +NNyhmMMd+:yMNs+yMy.      dMNMdmM",
  "  :Nh`oMMd. +Mm.`-MMm-   `sNMMs`hM",
  "   -dmNM+dm: oMNmNNsys--+hh/MNNhmM",
  "    `hMN+/dN/ +ssMm+yhhho- .My/dMM",
  "     `sMMmmMMdmd/hMMNmmNy- .Ms:hMM",
  "       +Ny-:MMm.`hMMMMMMMNs:hNNdNM",
  "        :mhyMdmd.`dMMMMMMMMNhs/`hM",
  "         .dMM/.hN:.MMMMMMMMMMMd+om",
  "          `yMNNmMN/hMMMMMMN/y+/+s:",
  "            oMh/oNN:MMd/:yM:ms-yNN",
  "           omMMy-MM+dM-   .-mNMmNM",
  "          sMMMMMMMMm:h     .MMMdNM"
};
static const std::string gBOOTUP_VERSION = "Version 0.2.2 $yAvaritia";
static const std::string gBOOTUP_COPYRIGHT = "(c) 2019 Longhorn Rocketry Association";

namespace {
  /**
   * @brief Run the integrated flight computer through a simulated flight.
   */
  static void run_sketch() {
    br("#b$w", '=', " #b$g>>> #b$wENTERING SIMULATION ", 0);

    // Initialize rocket FC
    setup();

    // Run sim until conclusion
    while (g_ff_simulator->is_running()) {
      g_ff_simulator->run(g_ff_config.simulation.dt);
      loop();
    }

    br("#b$w", '=', " $g<<< #b$wLEAVING SIMULATION  ", 0);

    // Print flight report
    FlightReport rep = g_ff_simulator->get_report();
    TELEM("Final rocket state: <$y" +
          std::to_string(rep.rocket_state.altitude) + "#r, $y" +
          std::to_string(rep.rocket_state.velocity) + "#r, $y" +
          std::to_string(rep.rocket_acceleration) + "#r>");
    TELEM("Simulation duration: $y" + std::to_string(rep.flight_duration) + "#r "
          + gUNIT_TIME);

    float apogee = rep.apogee - g_ff_config.simulation.initial_altitude;
    float apogee_ft = apogee * gMETERS_TO_FEET;

    TELEM("Apogee relative to GL: #b$c" + std::to_string(apogee) + "#r "
          + gUNIT_DISPLACEMENT + " (#b$c" + std::to_string(apogee_ft)
          + "#r ft)");

    float apogee_target = g_ff_config.simulation.target_altitude;
    float apogee_accuracy =
      (1 - fabs(rep.apogee - apogee_target) / apogee_target) * 100;

    outln(
      std::string("[#b$raimbot#r] Airbrake accuracy: #b$c" +
      std::to_string(apogee_accuracy)) + "%"
    );

    float max_mach = rep.max_velocity / aimbot::gMACH1;
    float max_g = rep.max_acceleration / atmos::gravity_at(0);

    TELEM("Max velocity: $y" + std::to_string(rep.max_velocity) + "#r "
          + gUNIT_VELOCITY + " ($y" + std::to_string(max_mach) + " #rM)");
    TELEM("Max acceleration: $y" + std::to_string(rep.max_acceleration) + "#r "
          + gUNIT_ACCEL + " ($y" + std::to_string(max_g) + " #rG)");
    TELEM("Time to apogee: $y" + std::to_string(rep.time_to_apogee) + "#r "
          + gUNIT_TIME);

    // Prompt for response
    TELEM("Enter Q to quit, or any other key to rerun");
    out("[#b$bffcore#r] > ");

    std::string in;
    std::getline(std::cin, in);

    if (in != "Q" && in != "q") {
      g_ff_simulator->reset();
      run_sketch();
    }
  }

  /**
   * @brief Displays the boot screen and runs the sim after a short time.
   */
  static void boot() {
    system("clear");

    std::vector<std::string> bootup_text;
    bootup_text.push_back("");
    bootup_text.push_back("#b$wFLIGHT FACTORY");
    bootup_text.push_back(gBOOTUP_VERSION);
    bootup_text.push_back(gBOOTUP_COPYRIGHT);

    unsigned int line_count = gBOOTUP_ASCII.size() + bootup_text.size();
    unsigned int gutter = (gTERMINAL_HEIGHT - line_count) / 2;

    for (unsigned int i = 0; i < gutter; i++)
      outln("");

    for (unsigned int i = 0; i < gBOOTUP_ASCII.size(); i++)
      outln_ctr("#b$w" + gBOOTUP_ASCII[i]);

    for (unsigned int i = 0; i < bootup_text.size(); i++)
      outln_ctr(bootup_text[i]);

    usleep(gBOOTUP_DURATION * 1e6);

    system("clear");
  }
}

void init(int k_argc, char** k_argv) {
  g_ff_initialized = true;

  boot();

  if (k_argc < 2) {
    TELEM("$rFATAL: No simulation file was provided");
    exit(1);
  }

  g_ff_fc_path = std::string(k_argv[1]);

  TELEM("Loading config from " + g_ff_fc_path + "...");

  g_ff_config = parse_ff_config(g_ff_fc_path + "/" + gFC_CONFIG_NAME);

  if (g_ff_config.simulation.type == DOF1)
    g_ff_simulator = new Dof1Simulator(g_ff_config);

  if (g_ff_simulator == nullptr) {
    TELEM("$rFATAL: No simulator could be built. Possible invalid type?");
    exit(1);
  }
}

void run() {
  if (!g_ff_initialized) {
    TELEM("$rFATAL: Cannot run simulation; Flight Factory is not initialized");
    return;
  }

  run_sketch();
}

void launch(int k_argc, char** k_argv) {
  init(k_argc, k_argv);
  run();
}

} // namespace ff
