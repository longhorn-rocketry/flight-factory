#include <chrono>
#include <ctime>

#include "noise.hpp"

unsigned int get_rand_seed() {
  return std::chrono::system_clock::now().time_since_epoch().count();
}
