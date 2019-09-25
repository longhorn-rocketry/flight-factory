#include "ffio.hpp"
#include "ff_telemetry.hpp"

namespace ff {

void topen(std::string k_name) {
  // Open the file and map the stream for future reads. Note that the stream
  // remains open until the program is terminated or tclose() is called.
  std::string path = g_ff_fc_path + "/" + g_tout + "/" + k_name + ".dat";
  std::ofstream* out = new std::ofstream(path);
  g_ff_tpipes[k_name] = out;
}

void tclose(std::string k_name) {
  // Close and unmap the stream
  std::ofstream* pipe = g_ff_tpipes[k_name];
  pipe->close();
  g_ff_tpipes.erase(g_ff_tpipes.find(k_name));
  delete pipe;
}

void tout(std::string k_pipe_name, std::string k_data) {
  (*g_ff_tpipes[k_pipe_name]) << k_data;
}

void tout(std::string k_pipe_name, float k_lhs, float k_rhs) {
  (*g_ff_tpipes[k_pipe_name]) << k_lhs << " " << k_rhs << std::endl;
}

} // namespace ff
