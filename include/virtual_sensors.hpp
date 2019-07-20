#ifndef FLIGHT_FACTORY_VIRTUAL_SENSORS_HPP
#define FLIGHT_FACTORY_VIRTUAL_SENSORS_HPP

#include "photonic.h"

class VirtualImu : public photonic::Imu {
protected:

public:
  VirtualImu();

  ~VirtualImu();

  bool initialize();

  void update();
};

class VirtualBarometer : public photonic::Barometer {
protected:

public:
  VirtualBarometer();

  ~VirtualBarometer();

  bool initialize();

  void update();
};

class VirtualTimekeeper : public photonic::Timekeeper {
public:
  float time();
};

#endif
