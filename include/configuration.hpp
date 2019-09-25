/**
 * Flight Factory global configuration.
 */
#ifndef FLIGHT_FACTORY_CONFIGURATION_HPP
#define FLIGHT_FACTORY_CONFIGURATION_HPP

#include "aimbot.hpp"
#include "motor.hpp"
#include "noise_generators.hpp"

/**
 * Types of flight simulators.
 */
enum SimulatorType {
  DOF1
};

/**
 * Events that can prompt simulator to exit.
 */
enum SimulatorStopCondition {
  STOP_CONDITION_APOGEE,
  STOP_CONDITION_IMPACT
};

/**
 * Models for calculating drag coefficients from rocket geometry and kinematic
 * state. These correspond models defined in Aimbot.
 */
enum CdModel {
  CD_MODEL_STATIC,
  CD_MODEL_SCHEDULE,
  CD_MODEL_PLANAR
};

/**
 * Models for computing drag force of an airbroken rocket. These correspond to
 * models defined in Aimbot.
 */
enum AirbrakeModel {
  AIRBRAKE_MODEL_STATIC,
  AIRBRAKE_MODEL_AIRFLOW_DEFLECTION
};

/**
 * Parameters governing simulator behavior and modeling.
 */
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

/**
 * Noise generators applied to a simulator.
 */
struct NoiseConfiguration {
  NoiseGenerator* sensor_accel = nullptr;
  NoiseGenerator* sensor_pressure = nullptr;
  NoiseGenerator* sensor_temperature = nullptr;
  NoiseGenerator* physics_accel = nullptr;
};

/**
 * Topmost configuration construct; completely defines the flight and rocket
 * being simulated. This is what gets generated from the .ff file.
 */
struct FlightFactoryConfiguration {
  SimulatorConfiguration simulation;
  NoiseConfiguration noise;
  aimbot::rocket_t rocket;
  motor_t motor;
};

#endif
