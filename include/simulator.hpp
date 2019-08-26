#ifndef FLIGHT_FACTORY_SIMULATOR_HPP
#define FLIGHT_FACTORY_SIMULATOR_HPP

#include <map>

#include "aimbot.hpp"
#include "barometer.h"
#include "configuration.hpp"
#include "imu.h"
#include "parser.hpp"

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
  std::map<std::string, float> m_parameters;
  aimbot::AirbrakeModel* m_airbrake_model;
  aimbot::CdModel* m_cd_model;

  float m_t_sim;
  float m_t_apogee;
  float m_apogee;
  float m_max_accel;
  float m_max_velocity;
  bool m_running;

public:
  Simulator(const FlightFactoryConfiguration& k_config);

  virtual void run(float dt) = 0;

  virtual photic::ImuData get_imu_data() = 0;

  virtual photic::BarometerData get_barometer_data() = 0;

  virtual aimbot::state_t get_rocket_state() = 0;

  virtual FlightReport get_report() = 0;

  virtual void reset() = 0;

  float get_time();

  bool is_running();

  float& operator[](std::string k_key);
};

#endif
