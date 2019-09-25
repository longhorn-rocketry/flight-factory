/**
 * Simulator hardware abstraction layer. This allows Photic-powered flight
 * computers to interface with FF simulators.
 */
#ifndef FLIGHT_FACTORY_VIRTUAL_SENSORS_HPP
#define FLIGHT_FACTORY_VIRTUAL_SENSORS_HPP

#include "photic.h"

/**
 * Virtual 9 DoF IMU.
 */
class VirtualImu : public photic::Imu {
public:
  VirtualImu();

  ~VirtualImu();

  bool initialize();

  void update();
};

/**
 * Virtual barometer measuring ambient pressure, temperature, and altitude.
 * Altitude is based off the Standard Atmosphere's prediction given pressure
 * and temperature.
 */
class VirtualBarometer : public photic::Barometer {
public:
  VirtualBarometer();

  ~VirtualBarometer();

  bool initialize();

  void update();
};

/**
 * Virtual timekeeper for generating time values. This is tied to simulator time
 * and not real (local machine) time.
 */
class VirtualTimekeeper : public photic::Timekeeper {
public:
  float time();
};

#endif
