#include "ff_arduino_harness.hpp"
#include "flight_factory.hpp"

VirtualSerial Serial;

VirtualSerial::VirtualSerial() {}

void VirtualSerial::println(std::string k_data) {
  ff::outln(k_data);
}
