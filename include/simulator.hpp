/**
 * Structures common to all simulators.
 */
#ifndef FLIGHT_FACTORY_SIMULATOR_HPP
#define FLIGHT_FACTORY_SIMULATOR_HPP

#include <map>

#include "aimbot.hpp"
#include "barometer.h"
#include "configuration.hpp"
#include "imu.h"
#include "parser.hpp"

/**
 * Safe noise generation for NoiseGenerator ptrs. Members of the global noise
 * configuration may be null, indicating no generator is configured.
 */
#define GEN_NOISE(gen_ptr) (gen_ptr == nullptr ? 0 : gen_ptr->get())

/**
 * Summary of a rocket flight.
 */
struct FlightReport {
  aimbot::state_t rocket_state;
  float rocket_acceleration;
  float flight_duration;
  float apogee;
  float target_apogee;
  float time_to_apogee;
  float max_acceleration;
  float max_velocity;
};

/**
 * Abstraction of a flight simulator.
 */
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
  /**
   * @brief Simulators begin with logical default parameters, e.g. vehicle is
   *        stationless on the launchpad.
   */
  Simulator(const FlightFactoryConfiguration& k_config);

  /**
   * Run a single simulator iteration.
   *
   * @param   dt step size in seconds
   */
  virtual void run(float dt) = 0;

  /**
   * @brief Gets IMU data as the flight computer should perceive it.
   */
  virtual photic::ImuData get_imu_data() = 0;

  /**
   * @brief Gets barometer data as the flight computer should perceive it.
   */
  virtual photic::BarometerData get_barometer_data() = 0;

  /**
   * @brief Gets the true state of the rocket. This will include environmental
   *        noise, but not sensor noise.
   */
  virtual aimbot::state_t get_rocket_state() = 0;

  /**
   * @brief Gets a report of the last flight.
   */
  virtual FlightReport get_report() = 0;

  /**
   * @brief Resets the simulator in anticipation of another flight.
   */
  virtual void reset() = 0;

  /**
   * @brief Gets the current simulator time. In general, this should be the
   *        mission time, with liftoff at t=0.
   */
  float get_time();

  /**
   * @brief Gets whether or not the simulator stop condition has been satisfied.
   */
  bool is_running();

  /**
   * @brief Sets/gets a simulator parameter.
   */
  float& operator[](std::string k_key);
};

#endif
