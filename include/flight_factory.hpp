#ifndef FLIGHT_FACTORY
#define FLIGHT_FACTORY

#include <iostream>

#include "aimbot.hpp"
#include "dof1_simulator.hpp"
#include "ffio.hpp"
#include "simulator.hpp"
#include "parser.hpp"
#include "photonic.h"

namespace ff {

/**
 * Global pointer to the simulation in progress. This should be used as a source
 * of mock sensor data for hardware abstractions.
 */
extern Simulator* g_ff_simulator;

/**
 * Global pointer to the factory configuration. Contains simulator and rocket
 * parameters, among other things.
 */
extern FlightFactoryConfiguration g_ff_config;

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
void run();

} // namespace ff

#endif
