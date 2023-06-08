#include "TrainData.h"

uint8_t TrainData::TrainCounter = 0;

TrainData::TrainData(const vector<uint8_t> macAddress, uint8_t buttonPin) : 
  mTrainMac(macAddress),
  mButtonPin(buttonPin), 
  mTrainId(TrainData::TrainCounter++),
  mCurrentStation(StationAnton),
  mIsRunning(false),
  mNextStationQueue() {}

uint8_t TrainData::getId() const {
  return mTrainId;
}

vector<uint8_t> TrainData::getMac() const {
  return mTrainMac;
}

uint8_t TrainData::getButtonPin() const {
  return mButtonPin;
}

StationId TrainData::getCurrentStation() const {
  return mCurrentStation;  
}

StationId TrainData::getNextStation() const {
  return mNextStationQueue.front();
}


void TrainData::pushDestination(StationId destination) {
  mNextStationQueue.push(destination);
}

void TrainData::popDestination() {
  mCurrentStation = mNextStationQueue.front();
  mNextStationQueue.pop();
}

bool TrainData::hasDestination() const {
  return !mNextStationQueue.empty();
}

bool TrainData::isRunning() const {
  return mIsRunning;
}

void TrainData::toggleRunning() {
  mIsRunning = !mIsRunning;
}

