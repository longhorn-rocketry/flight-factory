#include <unistd.h>

#include "ff_arduino_harness.hpp"
#include "flight_factory.hpp"

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
  void run_sketch() {
    br("#b$w", '=', " #b$g>>> ENTERING SIMULATION ", 0);

    // Initialize rocket FC
    setup();

    // Run sim until conclusion
    while (g_ff_simulator->is_running()) {
      g_ff_simulator->run(g_ff_config.simulation.dt);
      loop();
    }

    br("#b$w", '=', " $g<<< LEAVING SIMULATION ", 0);

    // Print flight report
    FlightReport rep = g_ff_simulator->get_report();
    CORE_TELEM("Final rocket state: <$y%f#r, $y%f#r, $y%f#r>",
               rep.rocket_state.altitude,
               rep.rocket_state.velocity,
               rep.rocket_acceleration);
    CORE_TELEM("Simulation duration: $y%f#r %s",
               rep.flight_duration,
               gUNIT_TIME.c_str());

    float apogee = rep.apogee - g_ff_config.simulation.initial_altitude;
    float apogee_ft = apogee * gMETERS_TO_FEET;

    CORE_TELEM("Apogee relative to GL: #b$c%f#r %s (#b$c%f#r ft)",
               apogee,
               gUNIT_DISPLACEMENT.c_str(),
               apogee_ft);

    float apogee_target = g_ff_config.simulation.target_altitude;
    float apogee_accuracy =
      (1 - fabs(rep.apogee - apogee_target) / apogee_target) * 100;

    out("[#b$raimbot#r] Airbrake accuracy: #b$c%f\%\n", apogee_accuracy);

    float max_mach = rep.max_velocity / aimbot::gMACH1;
    float max_g = rep.max_acceleration / atmos::gravity_at(0);

    CORE_TELEM("Max velocity: $y%f#r %s ($y%f#r M)",
               rep.max_velocity,
               gUNIT_VELOCITY.c_str(),
               max_mach);
    CORE_TELEM("Max acceleration: $y%f#r %s ($y%f#r G)",
               rep.max_acceleration,
               gUNIT_ACCEL.c_str(),
               max_g);
    CORE_TELEM("Time to apogee: $y%f#r %s",
               rep.time_to_apogee,
               gUNIT_TIME.c_str());

    // Prompt for response
    CORE_TELEM("Enter Q to quit, or any other key to rerun");
    out(std::string(CORE_TELEM_TAG) + "> ");

    std::string in;
    std::getline(std::cin, in);

    if (in != "Q" && in != "q") {
      photic::reset();
      g_ff_simulator->reset();
      run_sketch();
    }
  }

  /**
   * @brief Displays the boot screen and runs the sim after a short time.
   */
  void boot() {
    system("clear");

    std::vector<std::string> bootup_text;
    bootup_text.push_back("");
    bootup_text.push_back("#b$wFLIGHT FACTORY");
    bootup_text.push_back(gBOOTUP_VERSION);
    bootup_text.push_back(gBOOTUP_COPYRIGHT);

    unsigned int line_count = gBOOTUP_ASCII.size() + bootup_text.size();
    unsigned int gutter = (get_terminal_height() - line_count) / 2;

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
    CORE_TELEM("$rFATAL: No simulation file was provided");
    exit(1);
  }

  g_ff_fc_path = std::string(k_argv[1]);

  CORE_TELEM("Loading config from " + g_ff_fc_path + "...");

  g_ff_config = parse_ff_config(g_ff_fc_path + "/" + gFC_CONFIG_NAME);

  if (g_ff_config.simulation.type == DOF1)
    g_ff_simulator = new Dof1Simulator(g_ff_config);

  if (g_ff_simulator == nullptr) {
    CORE_TELEM("$rFATAL: No simulator could be built. Possible invalid type?");
    exit(1);
  }
}

void run() {
  if (!g_ff_initialized) {
    CORE_TELEM("$rFATAL: Cannot run simulation; Flight Factory is not initialized");
    return;
  }

  run_sketch();
}

void launch(int k_argc, char** k_argv) {
  init(k_argc, k_argv);
  run();
}

} // namespace ff
