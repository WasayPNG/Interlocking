#include "TrainData.h"

uint8_t TrainData::TrainCounter = 0;

TrainData::TrainData(const vector<uint8_t> macAddress, uint8_t buttonPin) : 
  mTrainMac(macAddress),
  mButton(buttonPin), 
  mTrainId(TrainData::TrainCounter++),
  mCurrentStation(StationAnton),
  mIsRunning(false),
  mWillArriveFirst(false),
  mIsWaitingAtSwitch(false),
  mNextStationQueue() {}

uint8_t TrainData::getId() const {
  return mTrainId;
}

vector<uint8_t> TrainData::getMac() const {
  return mTrainMac;
}

bool TrainData::isButtonPressed() {
  return mButton.isPressed();
}

StationId TrainData::getCurrentStation() const {
  return mCurrentStation;  
}

StationId TrainData::getNextStation() const {
  // queue<StationId> q = mNextStationQueue;
  // Serial.println("BEGIN STATION QUEUE:");
  // while (!q.empty()) {
  //   auto st = q.front();
  //   q.pop();
  //   Serial.println(st);
  // }
  // Serial.println(":END STATION QUEUE");
  if (mNextStationQueue.empty()) return StationInvalid;
  return mNextStationQueue.front();
}

StationId TrainData::getFinalStation() const {
  return mNextStationQueue.back();
}

void TrainData::pushDestination(StationId destination) {
  //printArgs("sisis", "Destination ", destination, " pushed for train ", mTrainId, "\n");
  mNextStationQueue.push(destination);
}

void TrainData::popDestination() {
  if (mNextStationQueue.empty()) return; /* return if empty */
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

bool TrainData::willArriveFirst() const {
  return mWillArriveFirst;
}

void TrainData::toggleArriveFirst() {
  mWillArriveFirst = !mWillArriveFirst;
}

bool TrainData::isWaitingAtSwitch() const {
  return mIsWaitingAtSwitch;
}

void TrainData::toggleWaitingAtSwitch() {
  mIsWaitingAtSwitch = !mIsWaitingAtSwitch;
}
