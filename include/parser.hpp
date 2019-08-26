#ifndef FLIGHT_FACTORY_PARSER_HPP
#define FLIGHT_FACTORY_PARSER_HPP

#include <iostream>
#include <fstream>

#include "configuration.hpp"

FlightFactoryConfiguration parse_ff_config(std::string k_fpath);

void parse_cd_plane(std::string k_fpath, FlightFactoryConfiguration& k_config);

#endif
