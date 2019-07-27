#include <math.h>

#include "dof1_simulator.hpp"
#include "flight_factory.hpp"
#include "standard_atmosphere.hpp"

#define INITIAL_ALTITUDE ff::g_ff_config.simulation.initial_altitude
#define T_IGNITION ff::g_ff_config.simulation.t_ignition
#define T_NO_STOP T_IGNITION * 2
#define ALTITUDE m_rocket_true_state.altitude
#define VELOCITY m_rocket_true_state.velocity
#define ACCEL m_rocket_acceleration

Dof1Simulator::Dof1Simulator() : Simulator() {
    reset();
}

void Dof1Simulator::reset() {
  ALTITUDE = INITIAL_ALTITUDE;
  VELOCITY = 0;
  ACCEL = 0;
  m_motor_burning = false;
  m_running = true;
  m_t_sim = 0;
}

void Dof1Simulator::compute_rocket_acceleration() {
  // Get base acceleration; gravity + drag
  if (ff::g_ff_config.cd_source == STATIC) {
    ACCEL = aimbot::simple_net_acceleration(
      ff::g_ff_config.rocket,
      m_rocket_true_state
    );
  } else if (ff::g_ff_config.cd_source == PROFILE) {
    ACCEL = aimbot::profiled_net_acceleration(
      ff::g_ff_config.rocket,
      m_rocket_true_state,
      ff::g_ff_config.cd_profile
    );
  } else if (ff::g_ff_config.cd_source == PLANE) {
    ACCEL = aimbot::planar_net_acceleration(
      ff::g_ff_config.rocket,
      m_rocket_true_state,
      ff::g_ff_config.cd_plane
    );
  }

  // Add in engine force
  float t_engine_burn = m_t_sim - T_IGNITION;
  float thrust = thrust_at(ff::g_ff_config.motor.thrust_profile, t_engine_burn);

  // Compute rocket mass
  float rocket_mass = ff::g_ff_config.rocket.mass;
  motor_t& motor = ff::g_ff_config.motor;

  if (t_engine_burn < 0)
    rocket_mass += motor.wet_mass;
  else if (t_engine_burn > motor.burn_time)
    rocket_mass += motor.dry_mass;
  else {
    float dm = motor.wet_mass - motor.dry_mass;
    float dt = 1 - t_engine_burn / motor.burn_time;
    rocket_mass += motor.dry_mass + dm * dt;
  }

  // Apply acceleration vector
  ACCEL += thrust / rocket_mass;
  m_motor_burning = thrust > 0;
}

void Dof1Simulator::run(float dt) {
  m_t_sim += dt;

  compute_rocket_acceleration();

  if (fabs(m_rocket_acceleration) > m_max_accel)
    m_max_accel = fabs(m_rocket_acceleration);

  VELOCITY += ACCEL * dt;
  ALTITUDE += VELOCITY * dt;

  if (fabs(VELOCITY) > m_max_velocity)
    m_max_velocity = fabs(VELOCITY);

  if (m_motor_burning) {
    if (VELOCITY < 0)
      VELOCITY = 0;
  }

  if (ALTITUDE < INITIAL_ALTITUDE)
    ALTITUDE = INITIAL_ALTITUDE;

  if (ALTITUDE > m_apogee) {
    m_apogee = ALTITUDE;
    m_t_apogee = m_t_sim;
  }

  // Stop conditions
  if (ff::g_ff_config.simulation.stop_condition == STOP_CONDITION_APOGEE &&
      VELOCITY <= 0 &&
      m_t_sim > T_NO_STOP)
  {
    m_running = false;
  } else if (ff::g_ff_config.simulation.stop_condition == STOP_CONDITION_IMPACT &&
             ALTITUDE == INITIAL_ALTITUDE &&
             m_t_sim > T_NO_STOP)
  {
    m_running = false;
  }
}

photonic::ImuData Dof1Simulator::get_imu_data() {
  photonic::ImuData data;

  data.ax = 0;
  data.ay = 0;
  data.az = ACCEL / atmos::gravity_at(0);
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

  data.altitude = ALTITUDE;
  data.pressure = atmos::pressure_at(data.altitude);
  data.temperature = atmos::temperature_at(data.altitude);

  return data;
}

aimbot::state_t Dof1Simulator::get_rocket_state() {
  return m_rocket_true_state;
}

FlightReport Dof1Simulator::get_report() {
  FlightReport rep;

  rep.rocket_state = m_rocket_true_state;
  rep.rocket_acceleration = m_rocket_acceleration;
  rep.flight_duration = m_t_sim - T_IGNITION;
  rep.apogee = m_apogee;
  rep.time_to_apogee = m_t_apogee - T_IGNITION;
  rep.max_acceleration = m_max_accel;
  rep.max_velocity = m_max_velocity;

  return rep;
}
