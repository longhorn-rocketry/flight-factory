#include "flight_factory.hpp"
#include "virtual_sensors.hpp"

VirtualImu::VirtualImu() {}

VirtualImu::~VirtualImu() {}

bool VirtualImu::initialize() {
  return true;
}

void VirtualImu::update() {
  data = ff::g_ff_simulator->get_imu_data();
}

VirtualBarometer::VirtualBarometer() {}

VirtualBarometer::~VirtualBarometer() {}

bool VirtualBarometer::initialize() {
  return true;
}

void VirtualBarometer::update() {
  data = ff::g_ff_simulator->get_barometer_data();
}

float VirtualTimekeeper::time() {
  return ff::g_ff_simulator->get_time();
}
