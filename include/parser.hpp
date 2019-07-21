#ifndef FLIGHT_FACTORY_PARSER_HPP
#define FLIGHT_FACTORY_PARSER_HPP

#include <iostream>
#include <fstream>

#include "aimbot.hpp"
#include "cd_profile.hpp"
#include "motor.hpp"
#include "simulator.hpp"

struct FlightFactoryConfiguration {
  SimulatorConfiguration simulation;
  aimbot::rocket_t rocket;
  motor_t motor;
  aimbot::cd_profile_t cd_profile;
};

FlightFactoryConfiguration parse_ff_config(std::string k_fpath);

#endif
