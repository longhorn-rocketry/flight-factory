#ifndef FLIGHT_FACTORY_VIRTUAL_SENSORS_HPP
#define FLIGHT_FACTORY_VIRTUAL_SENSORS_HPP

#include "photic.h"

class VirtualImu : public photic::Imu {
public:
  VirtualImu();

  ~VirtualImu();

  bool initialize();

  void update();
};

class VirtualBarometer : public photic::Barometer {
public:
  VirtualBarometer();

  ~VirtualBarometer();

  bool initialize();

  void update();
};

class VirtualTimekeeper : public photic::Timekeeper {
public:
  float time();
};

#endif
