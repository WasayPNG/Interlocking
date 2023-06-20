#ifndef STATION_H
#define STATION_H
#include <Arduino.h>
#include "Button.h"

enum StationId {
  StationAnton,
  StationBaker,
  StationCharlie,
  StationInvalid = -1
};

struct Station {
  StationId mId;
  Button mButton;
  Button mRelay;

  Station(StationId id, uint8_t buttonPin, uint8_t relayPin) : mId(id), mButton(buttonPin), mRelay(relayPin, DEFAULT_RELAY_DEBOUNCE) {}
};

#endif