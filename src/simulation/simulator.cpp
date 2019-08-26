#include "aimbot.hpp"
#include "simulator.hpp"
#include "static_cd_model.hpp"

#include "schedule_cd_model.hpp"
#include "planar_cd_model.hpp"

#include "static_airbrake_model.hpp"
#include "airflow_deflection_airbrake_model.hpp"

Simulator::Simulator(const FlightFactoryConfiguration& k_config) {
  m_t_sim = 0;
  m_t_apogee = 0;
  m_apogee = 0;
  m_max_accel = 0;
  m_max_velocity = 0;
  m_running = true;

  if (k_config.simulation.cd_model_type == CD_MODEL_STATIC)
    m_cd_model = new aimbot::StaticCdModel(k_config.rocket.drag_coefficient);
  else if (k_config.simulation.cd_model_type == CD_MODEL_SCHEDULE)
    m_cd_model = new aimbot::ScheduleCdModel(k_config.simulation.cd_profile);
  else if (k_config.simulation.cd_model_type == CD_MODEL_PLANAR)
    m_cd_model = new aimbot::PlanarCdModel(k_config.simulation.cd_plane);

  if (k_config.simulation.airbrake_model_type == AIRBRAKE_MODEL_STATIC)
    m_airbrake_model = new aimbot::StaticAirbrakeModel(m_cd_model);
  else if (k_config.simulation.airbrake_model_type ==
           AIRBRAKE_MODEL_AIRFLOW_DEFLECTION)
    m_airbrake_model =
        new aimbot::AirflowDeflectionAirbrakeModel(m_cd_model, 0.75);
}

float Simulator::get_time() {
  return m_t_sim;
}

bool Simulator::is_running() {
  return m_running;
}

float& Simulator::operator[](std::string k_key) {
  return m_parameters[k_key];
}
