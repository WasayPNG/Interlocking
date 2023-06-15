#ifndef STATION_H
#define STATION_H
#include <Arduino.h>
#include "Button.h"

enum StationId {
  StationAnton,
  StationBaker,
  StationCharlie,
};

struct Station {
  StationId mId;
  Button mButton;
  uint8_t mRelayPin;

  Station(StationId id, uint8_t buttonPin, uint8_t relayPin) : mId(id), mButton(buttonPin), mRelayPin(relayPin) {
    pinMode(relayPin, INPUT_PULLUP);
  }
};

#endif