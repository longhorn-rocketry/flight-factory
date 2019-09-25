/**
 * Simulation noise distributions.
 */
#ifndef FLIGHT_FACTORY_NOISE_HPP
#define FLIGHT_FACTORY_NOISE_HPP

/**
 * Abstraction of a noise source.
 */
class NoiseGenerator {
protected:

public:
  /**
   * @brief Generates a noise value.
   */
  virtual float get() = 0;
};

/**
 * @brief Gets a seed based on the current system time for random engines.
 */
unsigned int get_rand_seed();

#endif
