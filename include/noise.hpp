#ifndef FLIGHT_FACTORY_NOISE_HPP
#define FLIGHT_FACTORY_NOISE_HPP

class NoiseGenerator {
protected:

public:
  virtual float get() = 0;
};

unsigned int get_rand_seed();

#endif
