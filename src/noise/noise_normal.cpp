#include "noise_normal.hpp"

NormalNoiseGenerator::NormalNoiseGenerator(float k_var, float k_mean) :
  m_distribution(k_mean, k_var)
{
  m_generator = new std::mt19937(get_rand_seed());
}

NormalNoiseGenerator::~NormalNoiseGenerator() {
  delete m_generator;
}

float NormalNoiseGenerator::get() {
  return m_distribution(*m_generator);
}
