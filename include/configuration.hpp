#ifndef FLIGHT_FACTORY_CONFIGURATION_HPP
#define FLIGHT_FACTORY_CONFIGURATION_HPP

#include "aimbot.hpp"
#include "motor.hpp"

enum SimulatorType {
  DOF1
};

enum SimulatorStopCondition {
  STOP_CONDITION_APOGEE,
  STOP_CONDITION_IMPACT
};

enum CdModel {
  CD_MODEL_STATIC,
  CD_MODEL_SCHEDULE,
  CD_MODEL_PLANAR
};

enum AirbrakeModel {
  AIRBRAKE_MODEL_STATIC,
  AIRBRAKE_MODEL_AIRFLOW_DEFLECTION
};

struct SimulatorConfiguration {
  SimulatorType type;
  SimulatorStopCondition stop_condition;
  float initial_altitude;
  float target_altitude;
  float t_ignition;
  float dt;
  CdModel cd_model_type;
  AirbrakeModel airbrake_model_type;
  aimbot::cd_profile_t cd_profile;
  aimbot::cd_plane_t cd_plane;
};

struct FlightFactoryConfiguration {
  SimulatorConfiguration simulation;
  aimbot::rocket_t rocket;
  motor_t motor;
};

#endif
