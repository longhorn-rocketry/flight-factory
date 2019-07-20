#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

#include "ffio.hpp"
#include "parser.hpp"

#define TELEM(data) ff::outln(std::string("[#b$bffcore#r] ") + data)

static const char gSECTION_NAME_OPEN = '[';
static const char gSECTION_NAME_CLOSE = ']';
static const char gCOMMENT = '#';

static const std::string gSECTION_SIMULATION = "simulation";
static const std::string gSECTION_ROCKET = "rocket";
static const std::string gSECTION_CD_PROFILE = "cd";
static const std::string gSECTION_MOTOR_PROFILE = "motor";

namespace {
  void parse_pairing(const std::string& line,
                     std::string& key,
                     std::string& value,
                     char delim)
  {
    unsigned int delim_index = line.find(delim);
    key = line.substr(0, delim_index);
    value = line.substr(delim_index + 1, line.size() - delim_index - 1);
  }
}

FlightFactoryConfiguration parse_ff_config(std::string k_fpath) {
  FlightFactoryConfiguration config;

  std::ifstream in(k_fpath);
  std::string line;
  std::string current_section;

  std::vector<std::pair<float, float>> cd_profile;
  std::vector<std::pair<float, float>> motor_profile;

  while (std::getline(in, line)) {
    if (line.size() == 0)
      continue;

    char c = line.at(0);

    if (c == gCOMMENT)
      continue;
    else if (c == gSECTION_NAME_OPEN) {
      unsigned int closing_index = line.find(gSECTION_NAME_CLOSE, 1);
      current_section = line.substr(1, closing_index - 1);
    } else {
      std::string key, value;

      // Simulation parameters
      if (current_section == gSECTION_SIMULATION) {
        parse_pairing(line, key, value, '=');

        if (key == "type") {
          if (value == "dof1")
            config.simulation.type = DOF1;
        } else if (key == "initial_altitude") {
          config.simulation.initial_altitude = std::stof(value);
        } else if (key == "t_ignition") {
          config.simulation.t_ignition = std::stof(value);
        } else if (key == "dt") {
          config.simulation.dt = std::stof(value);
        } else if (key == "stop_condition") {
          if (value == "apogee")
            config.simulation.stop_condition = STOP_CONDITION_APOGEE;
          else if (value == "impact")
            config.simulation.stop_condition = STOP_CONDITION_IMPACT;
        }
      // Rocket parameters
      } else if (current_section == gSECTION_ROCKET) {
        parse_pairing(line, key, value, '=');

        if (key == "mass")
          config.rocket.mass = std::stof(value);
        else if (key == "radius")
          config.rocket.radius = std::stof(value);
        else if (key == "surface_area") {
          if (value == "auto")
            config.rocket.surface_area = 3.14159 * config.rocket.radius
                                         * config.rocket.radius;
          else
            config.rocket.surface_area = std::stof(value);
        } else if (key == "airbrake_surface_area")
          config.rocket.airbrake_surface_area = std::stof(value);
        else if (key == "drag_coefficient") {
          if (value != "auto")
            config.rocket.drag_coefficient = std::stof(value);
        } else if (key == "nosecone_length")
          config.rocket.nosecone_length = std::stof(value);
        else if (key == "fineness")
          config.rocket.fineness = std::stof(value);
        else if (key == "skin_roughness")
          config.rocket.skin_roughness = std::stof(value);
      // Cd profile
      } else if (current_section == gSECTION_CD_PROFILE) {
        parse_pairing(line, key, value, ' ');

        float a = std::stof(key);
        float b = std::stof(value);

        cd_profile.push_back(std::pair<float, float>(a, b));
      // Motor profile
      } else if (current_section == gSECTION_MOTOR_PROFILE) {
        parse_pairing(line, key, value, ' ');

        float a = std::stof(key);
        float b = std::stof(value);

        motor_profile.push_back(std::pair<float, float>(a, b));
      }
    }
  }

  // Build Cd and motor profiles
  config.cd_profile.events = new aimbot::cd_event_t[cd_profile.size()];
  config.cd_profile.size = cd_profile.size();

  for (unsigned int i = 0; i < cd_profile.size(); i++)
    config.cd_profile.events[i] = {cd_profile[i].first, cd_profile[i].second};

  if (cd_profile.size() > 0)
    TELEM("Parsed Cd profile with " + std::to_string(cd_profile.size()) + " events");

  config.motor_profile.events = new thrust_event_t[motor_profile.size()];
  config.motor_profile.size = motor_profile.size();

  for (unsigned int i = 0; i < motor_profile.size(); i++)
    config.motor_profile.events[i] = {motor_profile[i].first,
                                      motor_profile[i].second};

  if (motor_profile.size() > 0)
    TELEM("Parsed thrust profile with " + std::to_string(motor_profile.size()) + " events");

  return config;
}
