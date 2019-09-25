/**
 * Parser for .ff configuration files.
 */
#ifndef FLIGHT_FACTORY_PARSER_HPP
#define FLIGHT_FACTORY_PARSER_HPP

#include <iostream>
#include <fstream>

#include "configuration.hpp"

/**
 * @brief Extracts configuration from .ff files.
 */
FlightFactoryConfiguration parse_ff_config(std::string k_fpath);

/**
 * @brief Extracts a planar CD model from a plane source file.
 */
void parse_cd_plane(std::string k_fpath, FlightFactoryConfiguration& k_config);

#endif
