/**
 * Centermost module where the simulator lives.
 */
#ifndef FLIGHT_FACTORY
#define FLIGHT_FACTORY

#include <iostream>

#include "aimbot.hpp"
#include "dof1_simulator.hpp"
#include "ffio.hpp"
#include "simulator.hpp"
#include "parser.hpp"
#include "photic.h"

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

extern std::string g_ff_fc_path;

/**
 * Flight Factory core initialiation.
 *
 * @param   k_argc system argument count
 * @param   k_argv system argument array
 */
void init(int k_argc, char** k_argv);

/**
 * @brief Runs a simulation. Must be preceded by init().
 */
void run();

/**
 * @brief One-line init and run.
 */
void launch(int k_argc, char** k_argv);

} // namespace ff

#endif
