#include <math.h>
#include <iostream>

#include "dof1_simulator.hpp"
#include "flight_factory.hpp"
#include "standard_atmosphere.hpp"

#define INITIAL_ALTITUDE ff::g_ff_config.simulation.initial_altitude
#define T_IGNITION ff::g_ff_config.simulation.t_ignition
#define T_NO_STOP T_IGNITION * 2
#define ALTITUDE m_rocket_true_state.altitude
#define VELOCITY m_rocket_true_state.velocity
#define ACCEL m_rocket_acceleration

Dof1Simulator::Dof1Simulator(const FlightFactoryConfiguration& k_config) :
  Simulator(k_config)
{
    reset();
}

void Dof1Simulator::reset() {
  // Set vehicle to motionless on launchpad
  ALTITUDE = INITIAL_ALTITUDE;
  VELOCITY = 0;
  ACCEL = 0;

  m_motor_burning = false;
  m_running = true;
  m_t_sim = 0;
  m_max_accel = 0;
  m_max_velocity = 0;
  m_apogee = INITIAL_ALTITUDE;

  // Airbrake is retracted by default
  m_parameters["airbrake_extension"] = 0.0;
}

void Dof1Simulator::compute_rocket_acceleration() {
  ACCEL = -atmos::gravity_at(m_rocket_true_state.altitude);

  // Compute engine thrust
  float t_engine_burn = m_t_sim - T_IGNITION;
  float thrust = thrust_at(ff::g_ff_config.motor.thrust_profile, t_engine_burn);

  // Compute drag force
  float drag = m_airbrake_model->net_drag(ff::g_ff_config.rocket,
                                          m_rocket_true_state,
                                          m_parameters["airbrake_extension"]);

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
  ACCEL += (thrust - drag) / rocket_mass;
  // Add in noise
  ACCEL += GEN_NOISE(ff::g_ff_config.noise.physics_accel);
  m_motor_burning = thrust > 0;
}

void Dof1Simulator::run(float dt) {
  m_t_sim += dt;

  compute_rocket_acceleration();

  // Mark record high acceleration
  if (fabs(m_rocket_acceleration) > m_max_accel)
    m_max_accel = fabs(m_rocket_acceleration);

  // Advance state using simple Euler's method
  VELOCITY += ACCEL * dt;
  ALTITUDE += VELOCITY * dt;

  // Mark record high velocity
  if (fabs(VELOCITY) > m_max_velocity)
    m_max_velocity = fabs(VELOCITY);

  // Prevent rocket from falling during first moments of motor burn
  if (m_motor_burning) {
    if (VELOCITY < 0)
      VELOCITY = 0;
  }

  // Prevent rocket from falling through the ground
  if (ALTITUDE < INITIAL_ALTITUDE)
    ALTITUDE = INITIAL_ALTITUDE;

  // Mark record high altitude
  if (ALTITUDE > m_apogee) {
    m_apogee = ALTITUDE;
    m_t_apogee = m_t_sim;
  }

  // Evaluate stop conditions
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

photic::ImuData Dof1Simulator::get_imu_data() {
  photic::ImuData data;

  data.ax = 0 + GEN_NOISE(ff::g_ff_config.noise.sensor_accel);
  data.ay = 0 + GEN_NOISE(ff::g_ff_config.noise.sensor_accel);
  data.az = ACCEL / atmos::gravity_at(0)
            + GEN_NOISE(ff::g_ff_config.noise.sensor_accel);
  data.gx = 0;
  data.gy = 0;
  data.gz = 0;
  data.mx = 0;
  data.my = 0;
  data.mz = 0;

  return data;
}

photic::BarometerData Dof1Simulator::get_barometer_data() {
  photic::BarometerData data;

  data.altitude = ALTITUDE;
  data.pressure = atmos::pressure_at(data.altitude) / 0.10197
                  + GEN_NOISE(ff::g_ff_config.noise.sensor_pressure);
  data.temperature = atmos::temperature_at(data.altitude)
                     + GEN_NOISE(ff::g_ff_config.noise.sensor_temperature);

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
