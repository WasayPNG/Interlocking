#ifndef TRAIN_DATA_H
#define TRAIN_DATA_H
#include <queue>
#include "Station.h"
using namespace std;


class TrainData {
private:
  vector<uint8_t> mTrainMac;
  const uint8_t mTrainId;
  const uint8_t mButtonPin;
  StationId mCurrentStation;
  queue<StationId> mNextStationQueue;
  bool mIsRunning;

public:
  static uint8_t TrainCounter;
  TrainData(const vector<uint8_t>, uint8_t);

  uint8_t getId() const;
  vector<uint8_t> getMac() const;
  uint8_t getButtonPin() const;
  StationId getCurrentStation() const;
  StationId getNextStation() const;
  

  void pushDestination(StationId destination);
  void popDestination();

  bool hasDestination() const;
  bool isRunning() const;
  void toggleRunning();
};


#endif