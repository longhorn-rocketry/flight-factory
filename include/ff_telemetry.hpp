/**
 * Functions for permanent, mid-simulation telemetry logging.
 */
#ifndef FLIGHT_FACTORY_TELEMETRY_HPP
#define FLIGHT_FACTORY_TELEMETRY_HPP

#include <fstream>
#include <map>

#include "flight_factory.hpp"

namespace ff {

/**
 * Folder within sketch directory where telemetry logs will go.
 */
static const std::string g_tout = "telem";

/**
 * Map of telemetry log names to corresponding file outputs.
 */
static std::map<std::string, std::ofstream*> g_ff_tpipes;

/**
 * Opens a new telemetry log.
 *
 * @param   k_name log name
 */
void topen(std::string k_name);

/**
 * Closes a telemetry log.
 *
 * @param   k_name log name
 */
void tclose(std::string k_name);

/**
 * Logs a string.
 *
 * @param   k_pipe_name log name
 * @param   k_data      data
 */
void tout(std::string k_pipe_name, std::string k_data);

/**
 * Logs a pair of floats. More often than not, these are coordinates to be
 * visualized on a graph.
 *
 * @param   k_pipe_name stream name
 * @param   k_lhs       horizontal axis value
 * @param   k_rhs       vertical axis value
 */
void tout(std::string k_pipe_name, float k_lhs, float k_rhs);

} // namespace ff

#endif
