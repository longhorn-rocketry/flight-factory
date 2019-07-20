#include "ff_arduino_harness.hpp"
#include "flight_factory.hpp"

#define TELEM(data) outln(std::string("[#b$bffcore#r] ") + data)

namespace ff {

static const std::string gUNIT_DISPLACEMENT = "m";
static const std::string gUNIT_TIME = "s";
static const std::string gUNIT_VELOCITY = gUNIT_DISPLACEMENT + "/" + gUNIT_TIME;
static const std::string gUNIT_ACCEL = gUNIT_VELOCITY + "^2";

static const std::string gFC_CONFIG_NAME = ".ff";

static bool g_ff_initialized = false;
static std::string g_ff_node_name;

Simulator* g_ff_simulator = nullptr;
FlightFactoryConfiguration g_ff_config;

std::string g_ff_fc_path;

static const std::string gBOOTUP_ASCII =
R"(   ___  __   _____  ___   __    _____     ___  _      ___  _____  ___  __
  / __\/ /   \_   \/ _ \ / / /\/__   \   / __\/_\    / __\/__   \/___\/__\/\_/\
 / _\ / /     / /\/ /_\// /_/ /  / /\/  / _\ //_\\  / /     / /\//  // \//\_ _/
/ /  / /___/\/ /_/ /_\\/ __  /  / /    / /  /  _  \/ /___  / / / \_// _  \ / \
\/   \____/\____/\____/\/ /_/   \/     \/   \_/ \_/\____/  \/  \___/\/ \_/ \_/
)";
static const std::string gBOOTUP_VERSION = "$yVersion 0.0.0 #b$yAvaritia";

namespace {

  /**
   * @brief clears the terminal and prints the main banner
   */
  static void refresh_interface() {
    system("clear");
    // br("$c", '=');
    out("#b$b" + gBOOTUP_ASCII);
    // out("\n");
    outln_ctr("(c) 2019 Longhorn Rocketry Association");
    outln_ctr(gBOOTUP_VERSION);
    br("$c", '=');
  }

  /**
   * @brief emulate the Arduino sketch
   */
  static void run_sketch() {
    br("$c", '=');
    setup();

    // Run sim until conclusion
    while (g_ff_simulator->is_running()) {
      g_ff_simulator->run(g_ff_config.simulation.dt);
      loop();
    }

    // Print flight report
    br("$c", '=');

    FlightReport rep = g_ff_simulator->get_report();
    TELEM("Final rocket state: <$y" +
          std::to_string(rep.rocket_state.altitude) + "#r, $y" +
          std::to_string(rep.rocket_state.velocity) + "#r, $y" +
          std::to_string(rep.rocket_acceleration) + "#r>");
    TELEM("Flight duration: $y" + std::to_string(rep.flight_duration) + "#r "
          + gUNIT_TIME);

    float apogee = rep.apogee - g_ff_config.simulation.initial_altitude;

    TELEM("Apogee relative to GL: #b$c" + std::to_string(apogee) + "#r "
          + gUNIT_DISPLACEMENT);

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

    if (in != "Q") {
      g_ff_simulator->reset();
      run_sketch();
    }
  }
}

void init(int k_argc, char** k_argv, const char* k_node_name) {
  g_ff_initialized = true;
  g_ff_node_name = std::string(k_node_name);

  refresh_interface();

  if (k_argc < 2) {
    TELEM("$rFATAL: No simulation file was provided");
    exit(1);
  }

  g_ff_fc_path = std::string(k_argv[1]);

  TELEM("Loading config from $g" + g_ff_fc_path + "#r...");

  g_ff_config = parse_ff_config(g_ff_fc_path + "/" + gFC_CONFIG_NAME);

  if (g_ff_config.simulation.type == DOF1)
    g_ff_simulator = new Dof1Simulator();

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

} // namespace ff
