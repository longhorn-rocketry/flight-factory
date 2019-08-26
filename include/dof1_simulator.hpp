#ifndef FLIGHT_FACTORY_SIMULATOR_DOF1_HPP
#define FLIGHT_FACTORY_SIMULATOR_DOF1_HPP

#include "simulator.hpp"

class Dof1Simulator final : public Simulator {
private:
  aimbot::state_t m_rocket_true_state;
  float m_rocket_acceleration;
  bool m_motor_burning;

  void compute_rocket_acceleration();

public:
  Dof1Simulator(const FlightFactoryConfiguration& k_config);

  void run(float dt);

  photic::ImuData get_imu_data();

  photic::BarometerData get_barometer_data();

  aimbot::state_t get_rocket_state();

  FlightReport get_report();

  void reset();
};

#endif
