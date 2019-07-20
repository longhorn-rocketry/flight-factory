#ifndef FLIGHT_FACTORY_SIMULATOR_HPP
#define FLIGHT_FACTORY_SIMULATOR_HPP

#include "aimbot.hpp"
#include "barometer.h"
#include "imu.h"

enum SimulatorType {
  DOF1
};

struct SimulatorConfiguration {
  SimulatorType type;
  float initial_altitude;
  float t_ignition;
};

class Simulator {
protected:
  float m_t_sim;

public:
  Simulator(const SimulatorConfiguration& k_config);

  virtual void run(float dt) = 0;

  virtual photonic::ImuData get_imu_data() = 0;

  virtual photonic::BarometerData get_barometer_data() = 0;

  virtual aimbot::state_t get_rocket_state() = 0;

  float get_time();
};

#endif
