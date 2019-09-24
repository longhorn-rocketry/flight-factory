#include "noise_uniform.hpp"

UniformNoiseGenerator::UniformNoiseGenerator(float k_lower, float k_upper) :
  m_distribution(k_lower, k_upper)
{
  m_generator = new std::mt19937(get_rand_seed());
}

UniformNoiseGenerator::~UniformNoiseGenerator() {
  delete m_generator;
}

float UniformNoiseGenerator::get() {
  return m_distribution(*m_generator);
}
