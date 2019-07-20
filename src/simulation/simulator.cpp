#include "simulator.hpp"

Simulator::Simulator() {
  m_t_sim = 0;
  m_t_apogee = 0;
  m_apogee = 0;
  m_max_accel = 0;
  m_max_velocity = 0;
  m_running = true;
}

float Simulator::get_time() {
  return m_t_sim;
}

bool Simulator::is_running() {
  return m_running;
}
