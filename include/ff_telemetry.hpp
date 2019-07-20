#ifndef FLIGHT_FACTORY_TELEMETRY_HPP
#define FLIGHT_FACTORY_TELEMETRY_HPP

#include <fstream>
#include <map>

#include "flight_factory.hpp"

namespace ff {

static const std::string g_tout = "telem";

static std::map<std::string, std::ofstream*> g_ff_tpipes;

void topen(std::string k_name);

void tclose(std::string k_name);

void tout(std::string k_pipe_name, std::string k_data);

void tout(std::string k_pipe_name, float k_lhs, float k_rhs);

} // namespace ff

#endif
