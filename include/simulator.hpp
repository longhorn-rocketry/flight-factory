#ifndef FLIGHT_FACTORY_SIMULATOR_HPP
#define FLIGHT_FACTORY_SIMULATOR_HPP

#include "aimbot.hpp"
#include "barometer.h"
#include "imu.h"

enum SimulatorType {
  DOF1
};

enum SimulatorStopCondition {
  STOP_CONDITION_APOGEE,
  STOP_CONDITION_IMPACT
};

struct SimulatorConfiguration {
  SimulatorType type;
  SimulatorStopCondition stop_condition;
  float initial_altitude;
  float t_ignition;
  float dt;
};

struct FlightReport {
  aimbot::state_t rocket_state;
  float rocket_acceleration;
  float flight_duration;
  float apogee;
  float time_to_apogee;
  float max_acceleration;
  float max_velocity;
};

class Simulator {
protected:
  float m_t_sim;
  float m_t_apogee;
  float m_apogee;
  float m_max_accel;
  float m_max_velocity;
  bool m_running;

public:
  Simulator();

  virtual void run(float dt) = 0;

  virtual photonic::ImuData get_imu_data() = 0;

  virtual photonic::BarometerData get_barometer_data() = 0;

  virtual aimbot::state_t get_rocket_state() = 0;

  virtual FlightReport get_report() = 0;

  virtual void reset() = 0;

  float get_time();

  bool is_running();
};

#endif
