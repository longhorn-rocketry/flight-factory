#ifndef FLIGHT_FACTORY_MOTOR_HPP
#define FLIGHT_FACTORY_MOTOR_HPP

#include "struct_util.hpp"

typedef struct {
  float t;
  float force;
} thrust_event_t;

typedef struct {
  thrust_event_t* events;
  unsigned int size;
} thrust_profile_t;

float thrust_at(const thrust_profile_t& k_profile, float k_t);

#endif
