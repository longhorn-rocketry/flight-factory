#include "simulator.hpp"

Simulator::Simulator(const SimulatorConfiguration& k_config) {
  m_t_sim = 0;
}

float Simulator::get_time() {
  return m_t_sim;
}
