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
static const std::string gSECTION_CD_PROFILE = "cd_profile";
static const std::string gSECTION_CD_PLANE = "cd_plane";
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
        } else if (key == "target_altitude") {
          config.simulation.target_altitude = std::stof(value);
        } else if (key == "t_ignition") {
          config.simulation.t_ignition = std::stof(value);
        } else if (key == "dt") {
          config.simulation.dt = std::stof(value);
        } else if (key == "stop_condition") {
          if (value == "apogee")
            config.simulation.stop_condition = STOP_CONDITION_APOGEE;
          else if (value == "impact")
            config.simulation.stop_condition = STOP_CONDITION_IMPACT;
        } else if (key == "cd_model") {
          if (value == "static")
            config.simulation.cd_model_type = CD_MODEL_STATIC;
          else if (value == "profile")
            config.simulation.cd_model_type = CD_MODEL_SCHEDULE;
          else if (value == "planar")
            config.simulation.cd_model_type = CD_MODEL_PLANAR;
          else
            TELEM("$rWarning: unknown Cd model \"" + value + "\"; using static");
        } else if (key == "airbrake_model") {
          if (value == "static")
            config.simulation.airbrake_model_type = AIRBRAKE_MODEL_STATIC;
          else if (value == "airflow_deflection")
            config.simulation.airbrake_model_type =
                AIRBRAKE_MODEL_AIRFLOW_DEFLECTION;
          else
            TELEM("$rWarning: unknown airbrake model \"" + value + "\"; using static");
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
        else if (key == "drag_coefficient")
          config.rocket.drag_coefficient = std::stof(value);
        else if (key == "nose_cone_length")
          config.rocket.nose_cone_length = std::stof(value);
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
        // Thrust profile points
        if (line.find(' ') != std::string::npos) {
          parse_pairing(line, key, value, ' ');

          float a = std::stof(key);
          float b = std::stof(value);

          motor_profile.push_back(std::pair<float, float>(a, b));
        // Motor properties
        } else {
          parse_pairing(line, key, value, '=');

          if (key == "wet_mass")
            config.motor.wet_mass = std::stof(value);
          else if (key == "dry_mass")
            config.motor.dry_mass = std::stof(value);
        }
      // Cd plane
      } else if (current_section == gSECTION_CD_PLANE) {
        parse_pairing(line, key, value, '=');

        if (key == "src")
          parse_cd_plane(value, config);
      }
    }
  }

  // Build Cd and motor profiles
  config.simulation.cd_profile.events =
    new aimbot::cd_event_t[cd_profile.size()];
  config.simulation.cd_profile.size = cd_profile.size();

  for (unsigned int i = 0; i < cd_profile.size(); i++)
    config.simulation.cd_profile.events[i] =
      {cd_profile[i].first, cd_profile[i].second};

  if (cd_profile.size() > 0)
    TELEM("Parsed Cd profile with " + std::to_string(cd_profile.size()) + " events");

  config.motor.thrust_profile.events =
      new thrust_event_t[motor_profile.size()];
  config.motor.thrust_profile.size = motor_profile.size();

  for (unsigned int i = 0; i < motor_profile.size(); i++)
    config.motor.thrust_profile.events[i] = {motor_profile[i].first,
                                             motor_profile[i].second};

  if (motor_profile.size() > 0) {
    config.motor.burn_time = motor_profile[motor_profile.size() - 1].first;

    TELEM("Parsed thrust profile with " + std::to_string(motor_profile.size()) + " events");
  }

  return config;
}

void parse_cd_plane(std::string k_fpath, FlightFactoryConfiguration& k_config) {
  std::ifstream in(k_fpath);
  std::string line;
  std::string current_section;
  std::vector<float> coeffs;

  while (std::getline(in, line)) {
    if (line.size() == 0)
      continue;

    if (line.at(0) == gCOMMENT)
      continue;

    // Cd value
    if (line.find("=") == std::string::npos) {
      float cd = std::stof(line);
      coeffs.push_back(cd);
    // Plane parameter
    } else {
      std::string key, value;
      parse_pairing(line, key, value, '=');

      float* f;

      if (key == "s_low")
        f = &k_config.simulation.cd_plane.alt_low;
      else if (key == "s_high")
        f = &k_config.simulation.cd_plane.alt_high;
      else if (key == "s_step")
        f = &k_config.simulation.cd_plane.alt_step;
      else if (key == "v_low")
        f = &k_config.simulation.cd_plane.vel_low;
      if (key == "v_high")
        f = &k_config.simulation.cd_plane.vel_high;
      else if (key == "v_step")
        f = &k_config.simulation.cd_plane.vel_step;

      *f = std::stof(value);
    }
  }

  k_config.simulation.cd_plane.plane = new float[coeffs.size()];

  for (std::size_t i = 0; i < coeffs.size(); i++)
    k_config.simulation.cd_plane.plane[i] = coeffs[i];

  TELEM("Parsed Cd plane with " + std::to_string(coeffs.size()) + " points");

  in.close();
}
