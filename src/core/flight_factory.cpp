#include "ff_arduino_harness.hpp"
#include "flight_factory.hpp"

namespace ff {

static bool g_ff_initialized = false;
static std::string g_ff_node_name;

static const char gMETACHAR_COLOR = '$';
static const char gMETACHAR_FORMAT = '#';
static const char gMETACHAR_MULTICODE_OPEN = '{';
static const char gMETACHAR_MULTICODE_CLOSE = '}';

static const unsigned int gTERMINAL_WIDTH = 80;

static const std::string gBOOTUP_ASCII =
R"(   ___  __   _____  ___   __    _____     ___  _      ___  _____  ___  __
  / __\/ /   \_   \/ _ \ / / /\/__   \   / __\/_\    / __\/__   \/___\/__\/\_/\
 / _\ / /     / /\/ /_\// /_/ /  / /\/  / _\ //_\\  / /     / /\//  // \//\_ _/
/ /  / /___/\/ /_/ /_\\/ __  /  / /    / /  /  _  \/ /___  / / / \_// _  \ / \
\/   \____/\____/\____/\/ /_/   \/     \/   \_/ \_/\____/  \/  \___/\/ \_/ \_/
)";
static const std::string gBOOTUP_VERSION = "$yVersion 0.0.0 #b$yAvaritia";
static const std::string gCORE_TELEM_PREFIX = "[#b$bffcore#r] ";

Simulator* g_ff_simulator = nullptr;

static std::map<std::string, std::string> g_color_codes = {
  {"k", "30"},
  {"r", "31"},
  {"g", "32"},
  {"y", "33"},
  {"b", "34"},
  {"m", "35"},
  {"c", "36"},
  {"w", "37"}
};
static std::map<std::string, std::string> g_format_codes = {
  {"r", "0"},
  {"b", "1"},
  {"u", "4"},
  {"i", "7"},
  {"bo", "21"},
  {"uo", "24"},
  {"io", "27"},
};

namespace {

  /**
   * Parse color or format code information from a string.
   *
   * @param k_data      string to parse
   * @param k_start_pos position to begin parse at
   * @param k_code      string to put clean code into
   * @param k_offset    size of code, including formatting metacharacters
   */
  static void parse_code(const std::string& k_data,
                         unsigned int k_start_pos,
                         std::string& k_code,
                         unsigned int& k_offset)
  {
    k_offset = 1;
    char c_next = k_data.at(k_start_pos + 1);

    if (c_next == gMETACHAR_MULTICODE_OPEN) {
      unsigned int open_pos = k_start_pos + 2;
      unsigned int close_pos = k_data.find(gMETACHAR_MULTICODE_CLOSE,
                                           k_start_pos);
      k_code = k_data.substr(open_pos, close_pos - open_pos);
      k_offset += k_code.size() + 2;
    } else {
      k_code += c_next;
      k_offset++;
    }
  }

  /**
   * Gets the length of a string not including metacharacters and formatting
   * codes.
   *
   * @param  k_data string to evaluate
   * @return        true string length
   */
  static int strlen_no_metas(const std::string& k_data) {
    unsigned int pos = 0;
    unsigned int size = 0;

    while (pos < k_data.size()) {
      char c = k_data.at(pos);

      if (c == gMETACHAR_COLOR || c == gMETACHAR_FORMAT) {
        std::string code;
        unsigned int offset;

        parse_code(k_data, pos, code, offset);

        pos += offset;
      } else {
        pos++;
        size++;
      }
    }

    return size;
  }

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
   * @brief print some telemetry with the core prefix
   */
  static void telem(std::string k_data) {
    outln(gCORE_TELEM_PREFIX + k_data);
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

void out(std::string k_data) {
  unsigned int pos = 0;
  std::string format_code = g_format_codes[std::string("r")];

  while (pos < k_data.size()) {
    char c = k_data.at(pos);

    if (c == gMETACHAR_FORMAT) {
      std::string code;
      unsigned int offset;

      parse_code(k_data, pos, code, offset);

      format_code = g_format_codes[code];
      std::cout << "\033[" << format_code << "m";
      pos += offset;
    } else if (c == gMETACHAR_COLOR) {
      std::string code;
      unsigned int offset;

      parse_code(k_data, pos, code, offset);

      std::string ansi = g_color_codes[code];
      std::cout << "\033[" << format_code << ";" << ansi << "m";
      pos += offset;
    } else {
      std::cout << c;
      pos++;
    }
  }

  std::cout << "\033[0m";
}

void outln(std::string k_data) {
  out(k_data + "\n");
}

void outln_ctr(std::string k_data) {
  unsigned int gutter = (gTERMINAL_WIDTH - strlen_no_metas(k_data)) / 2;

  for (unsigned int i = 0; i < gutter; i++)
    out(" ");

  outln(k_data);
}

void br(std::string code, char c) {
  for (int i = 0; i < gTERMINAL_WIDTH; i++)
    out(code + c);

  out("\n");
}

void init(int k_argc, char** k_argv, const char* k_node_name) {
  g_ff_initialized = true;
  g_ff_node_name = std::string(k_node_name);

  refresh_interface();
}

void run(const aimbot::rocket_t k_rocket,
         const SimulatioNConfiguration& k_sim_config)
{
  if (!gg_ff_initialized) {
    telem("$rFATAL: Cannot run simulation; Flight Factory is not initialized");
    return;
  }

  run_sketch();
}

} // namespace ff
