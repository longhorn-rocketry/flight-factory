#ifndef FLIGHT_FACTORY_NOISE_UNIFORM_HPP
#define FLIGHT_FACTORY_NOISE_UNIFORM_HPP

#include <random>

#include "noise.hpp"

class UniformNoiseGenerator : public NoiseGenerator {
protected:
  std::mt19937* m_generator;
  std::uniform_real_distribution<float> m_distribution;

public:
  UniformNoiseGenerator(float k_lower, float k_upper);

  ~UniformNoiseGenerator();

  float get();
};

#endif
