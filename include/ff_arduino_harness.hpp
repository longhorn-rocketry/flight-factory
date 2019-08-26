#ifndef FLIGHT_FACTORY_ARDUINO_HARNESS_HPP
#define FLIGHT_FACTORY_ARDUINO_HARNESS_HPP

#include <string>

#include "aimbot.hpp"
#include "ff_telemetry.hpp"
#include "flight_factory.hpp"
#include "simulator.hpp"
#include "virtual_sensors.hpp"

#define SIM (*ff::g_ff_simulator)

class VirtualSerial {
public:
  VirtualSerial();

  void println(std::string k_data);
};

extern VirtualSerial Serial;

void setup();

void loop();

#endif
