#include "flight_factory.hpp"
#include "ff_arduino_harness.hpp"

#include "motor.hpp"

using namespace photonic;

int main(int argc, char** argv) {
  ff::init(argc, argv, "ff");
  ff::run();
}
