/**
 * Main point of integration between simulator and flight computer. Provides
 * prototypes for Arduino sketch methods and overshadows for certain Arduino
 * objects.
 */
#ifndef FLIGHT_FACTORY_ARDUINO_HARNESS_HPP
#define FLIGHT_FACTORY_ARDUINO_HARNESS_HPP

#include <string>

#include "aimbot.hpp"
#include "ff_telemetry.hpp"
#include "flight_factory.hpp"
#include "simulator.hpp"
#include "virtual_sensors.hpp"

#define SIM (*ff::g_ff_simulator)

/**
 * Overshadow of Arduino's static Serial class.
 */
class VirtualSerial {
public:
  VirtualSerial();

  void println(std::string k_data);
};

extern VirtualSerial Serial;

/**
 * One-time setup function.
 */
void setup();

/**
 * Iterative loop function called as fast as possible following setup().
 */
void loop();

#endif
