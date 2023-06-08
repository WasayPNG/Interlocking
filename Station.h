#ifndef STATION_H
#define STATION_H

enum StationId {
  StationAnton,
  StationBaker,
  StationCharlie,
};

struct Station {
  StationId mId;
  uint8_t mButtonPin;
};

#endif