/**
 * Structures for configuring rocket motors.
 */
#ifndef FLIGHT_FACTORY_MOTOR_HPP
#define FLIGHT_FACTORY_MOTOR_HPP

#include "struct_util.hpp"

/**
 * A thrust scalar delivered at a point in time.
 */
typedef struct {
  float t;
  float force;
} thrust_event_t;

/**
 * A collection of thrust events; thrust scalars parametrized by time.
 */
typedef struct {
  thrust_event_t* events;
  unsigned int size;
} thrust_profile_t;

/**
 * Motor specifications.
 */
typedef struct {
  thrust_profile_t thrust_profile;
  float wet_mass;
  float dry_mass;
  float burn_time;
} motor_t;

/**
 * Interpolates the force being produced at a point in a thrust profile.
 *
 * @param   k_profile profile to examine
 * @param   k_t       time relative to the start of the profile
 *
 * @ret     thrust
 */
float thrust_at(const thrust_profile_t& k_profile, float k_t);

#endif
