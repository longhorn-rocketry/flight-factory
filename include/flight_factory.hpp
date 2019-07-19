#ifndef FLIGHT_FACTORY
#define FLIGHT_FACTORY

#include <iostream>
#include <map>
#include <string>

#include "aimbot.hpp"
#include "dof1_simulator.hpp"
#include "photonic.h"

namespace ff {

/**
 * Global pointer to the simulation in progress. This should be used as a source
 * of mock sensor data for hardware abstractions.
 */
extern Simulator* g_ff_simulator;

/**
 * Stdout for Flight Factory. Supports formatting codes.
 *
 * # followed by a code indicates a format change. $ followed by a code
 * indicates a color change. See maps at the top for valid codes. Format
 * changes must preceed color changes. Multicharacter codes must be clasped in
 * curly brackets {}.
 *
 * @param k_data string to print
 */
void out(std::string k_data);

/**
 * Stdout with an appended newline. Supports formatting codes. See ff:out for
 * details.
 *
 * @param k_data string to print
 */
void outln(std::string k_data);

/**
 * Identical to outln but centers the printed line in the terminal.
 *
 * @param k_data string to print
 */
void outln_ctr(std::string k_data);

/**
 * Prints a horizontal rule.
 *
 * @param code formatting code
 * @param c    rule character
 */
void br(std::string code, char c);

/**
 * Flight Factory core initialiation.
 *
 * @param k_argc      system argument count
 * @param k_argv      system argument array
 * @param k_node_name name of client node (completely arbitrary and only for
 *                    added telemetry fanciness)
 */
void init(int k_argc, char** k_argv, const char* k_node_name);

/**
 * @brief runs a simulation
 */
void run(const aimbot::rocket_t k_rocket,
         const SimulatioNConfiguration& k_sim_config);

} // namespace ff

#endif
