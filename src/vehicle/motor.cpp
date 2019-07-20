#include "motor.hpp"
#include "standard_atmosphere.hpp"
#include "struct_util.hpp"

#include <iostream>

float thrust_at(const thrust_profile_t& k_profile, float k_t) {
  if (k_t <= k_profile.events[0].t)
    return k_profile.events[0].force;
  else if (k_t >= k_profile.events[k_profile.size - 1].t)
    return k_profile.events[k_profile.size - 1].force;

  unsigned int pos = aimbot::float_struct_binsearch(&k_profile,
                                                    k_t,
                                                    k_profile.size,
                                                    sizeof(thrust_profile_t),
                                                    0);
  const thrust_event_t& event_low = k_profile.events[pos - 1];
  const thrust_event_t& event_high = k_profile.events[pos];

  float thrust_low = event_low.force;
  float thrust_high = event_high.force;
  float p = (k_t - event_low.t) / (event_high.t - event_low.t);

  return atmos::interpolate_linear(thrust_low, thrust_high, p);
}
