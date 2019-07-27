#ifndef FLIGHT_FACTORY_PARSER_HPP
#define FLIGHT_FACTORY_PARSER_HPP

#include <iostream>
#include <fstream>

#include "aimbot.hpp"
#include "cd_plane.hpp"
#include "cd_profile.hpp"
#include "motor.hpp"
#include "simulator.hpp"

enum CdSource {
  STATIC,
  PROFILE,
  PLANE
};

struct FlightFactoryConfiguration {
  SimulatorConfiguration simulation;
  aimbot::rocket_t rocket;
  motor_t motor;
  aimbot::cd_profile_t cd_profile;
  aimbot::cd_plane_t cd_plane;
  CdSource cd_source;
};

FlightFactoryConfiguration parse_ff_config(std::string k_fpath);

void parse_cd_plane(std::string k_fpath, FlightFactoryConfiguration& k_config);

#endif
