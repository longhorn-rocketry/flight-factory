#ifndef SERAPH_ABC_CONFIG_H
#define SERAPH_ABC_CONFIG_H

#include "config.hpp"
#include "drag_force.hpp"
#include "seraph_cdp.h"

namespace seraph_vehicle {

const float g_AIRFLOW_DEFLECTION_RHO = 0.75;

aimbot::abc_config_t abc_config() {
  aimbot::abc_config_t conf;

  conf.target_altitude = 5000; // 1293.876;

  conf.bs_profile_velocity_min = 25;
  conf.bs_profile_velocity_max = 300;
  conf.bs_profile_step_min = 0.075;
  conf.bs_profile_step_max = 0.1;
  conf.bs_profile_exp = -1;

  conf.bsc_thresh_oscillation = 0.7;
  conf.bsc_thresh_stability = 0.2;
  conf.bsc_error_history_size = 4;

  conf.bsc_damp_profile_weight_min = 0.1;
  conf.bsc_damp_profile_weight_max = 0.3;
  conf.bsc_damp_profile_velocity_min = 40;
  conf.bsc_damp_profile_velocity_max = 250;
  conf.bsc_damp_profile_exp = -1;

  conf.bsc_amplify_profile_weight_min = 0.1;
  conf.bsc_amplify_profile_weight_max = 0.3;
  conf.bsc_amplify_profile_velocity_min = 40;
  conf.bsc_amplify_profile_velocity_max = 250;
  conf.bsc_amplify_profile_exp = -1;

  return conf;
}

aimbot::rocket_t rocket() {
  aimbot::rocket_t rocket;

  rocket.mass = 23.5;
  rocket.radius = 0.0762;
  rocket.surface_area = rocket.radius * rocket.radius * 3.14159;
  rocket.airbrake_surface_area = 0.0070866;
  rocket.drag_coefficient = 0.46;

  return rocket;
}

} // end namespace seraph

#endif
