#ifndef TRAIN_DATA_H
#define TRAIN_DATA_H
#include <queue>
#include "Station.h"
#include "Button.h"
using namespace std;


class TrainData {
private:
  vector<uint8_t> mTrainMac;
  const uint8_t mTrainId;
  Button mButton;
  StationId mCurrentStation;
  queue<StationId> mNextStationQueue;
  bool mIsRunning;
  bool mWillArriveFirst;

public:
  static uint8_t TrainCounter;
  TrainData(const vector<uint8_t>, uint8_t);

  uint8_t getId() const;
  vector<uint8_t> getMac() const;
  bool isButtonPressed();
  StationId getCurrentStation() const;
  StationId getNextStation() const;
  StationId getFinalStation() const;
  
  void pushDestination(StationId destination);
  void popDestination();

  bool hasDestination() const;
  bool isRunning() const;
  void toggleRunning();
  bool willArriveFirst() const;
  void toggleArriveFirst();
};


#endif