#include "ff_arduino_harness.hpp"
#include "flight_factory.hpp"
#include "parser.hpp"

#define TELEM(data) outln(std::string("[#b$bffcore#r] ") + data)

namespace ff {

static bool g_ff_initialized = false;
static std::string g_ff_node_name;

Simulator* g_ff_simulator = nullptr;
FlightFactoryConfiguration g_ff_config;

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
    setup();

    while (true)
      loop();
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

  TELEM("Loading config from $g" + std::string(k_argv[1]) + "#r...");

  g_ff_config = parse_ff_config(std::string(k_argv[1]));

  float f = thrust_at(g_ff_config.motor_profile, 1.5);
}

void run() {
  if (!g_ff_initialized) {
    TELEM("$rFATAL: Cannot run simulation; Flight Factory is not initialized");
    return;
  }

  run_sketch();
}

} // namespace ff
