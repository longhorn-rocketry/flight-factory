#include "dof1_simulator.hpp"
#include "standard_atmosphere.hpp"

Dof1Simulator::Dof1Simulator(const SimulatorConfiguration& k_config,
                             const aimbot::rocket_t k_rocket) :
  Simulator(k_config), mCONFIG(k_config), mROCKET_PROPERTIES(k_rocket)
{
    m_rocket_true_state.altitude = k_config.initial_altitude;
    m_rocket_true_state.velocity = 0;
    m_rocket_acceleration = 0;
}

void Dof1Simulator::compute_rocket_acceleration() {
  // Get base acceleration; gravity + drag
  m_rocket_acceleration = aimbot::simple_net_acceleration(mROCKET_PROPERTIES,
                                                          m_rocket_true_state);
  // Add in engine force
  // TODO wtfffffffffffffff
}

void Dof1Simulator::run(float dt) {
  m_t_sim += dt;

  compute_rocket_acceleration();

  m_rocket_true_state.velocity += m_rocket_acceleration * dt;
  m_rocket_true_state.altitude += m_rocket_true_state.velocity * dt;

  // Prevent rocket from falling through the ground
  if (m_rocket_true_state.altitude < mCONFIG.initial_altitude) {
    m_rocket_true_state.altitude = mCONFIG.initial_altitude;
    m_rocket_true_state.velocity = 0;
  }
}

photonic::ImuData Dof1Simulator::get_imu_data() {
  photonic::ImuData data;

  data.ax = 0;
  data.ay = 0;
  data.az = m_rocket_acceleration;
  data.gx = 0;
  data.gy = 0;
  data.gz = 0;
  data.mx = 0;
  data.my = 0;
  data.mz = 0;

  return data;
}

photonic::BarometerData Dof1Simulator::get_barometer_data() {
  photonic::BarometerData data;

  data.altitude = m_rocket_true_state.altitude;
  data.pressure = atmos::pressure_at(data.altitude);
  data.temperature = atmos::temperature_at(data.altitude);

  return data;
}

aimbot::state_t Dof1Simulator::get_rocket_state() {
  return m_rocket_true_state;
}
