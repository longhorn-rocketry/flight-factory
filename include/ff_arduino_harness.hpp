#ifndef FLIGHT_FACTORY_ARDUINO_HARNESS_HPP
#define FLIGHT_FACTORY_ARDUINO_HARNESS_HPP

#include <string>

class VirtualSerial {
public:
  VirtualSerial();

  void println(std::string k_data);
};

extern VirtualSerial Serial;

void setup();

void loop();

#endif
