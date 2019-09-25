/**
 * Noise generator with a normal distribution. Uses a Marsenne Twister seeded
 * with the current system time.
 */
#ifndef FLIGHT_FACTORY_NOISE_NORMAL_HPP
#define FLIGHT_FACTORY_NOISE_NORMAL_HPP

#include <random>

#include "noise.hpp"

class NormalNoiseGenerator : public NoiseGenerator {
protected:
  std::mt19937* m_generator;
  std::normal_distribution<float> m_distribution;

public:
  /**
   * @param   k_var  distribution variance
   * @param   k_mean distribution mean
   */
  NormalNoiseGenerator(float k_var, float k_mean);

  ~NormalNoiseGenerator();

  float get();
};

#endif
