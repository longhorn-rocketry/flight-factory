#ifndef FLIGHT_FACTORY_ARDUINO_HARNESS_HPP
#define FLIGHT_FACTORY_ARDUINO_HARNESS_HPP

#include <string>

#include "aimbot.hpp"
#include "simulator.hpp"

extern const aimbot::rocket_t gROCKET_PROPERTIES;
extern const SimulatorConfiguration gSIMULATION_CONFIGURATION;

class VirtualSerial {
public:
  VirtualSerial();

  void println(std::string k_data);
};

extern VirtualSerial Serial;

void setup();

void loop();

#endif
