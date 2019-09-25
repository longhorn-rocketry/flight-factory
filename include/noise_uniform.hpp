/**
 * Noise generator with a uniform distribution. Uses a Marsenne Twister seeded
 * with the current system time.
 */
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
