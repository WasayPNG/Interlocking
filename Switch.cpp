#include "Switch.h"

bool Route::isEqual(const Route &r) {
  return (mCurrent == r.mCurrent && mNext == r.mNext);
}

uint8_t Switch::SwitchCounter = 0;

Switch::Switch(uint8_t servoPin, vector<RelaySwitchPosition> relayPins, vector<RouteConfig> routes) :
  mServo(),
  mServoPin(servoPin),
  mRelayPositions(relayPins),
  mSwitchId(Switch::SwitchCounter++),
  mPositionQueue(),
  mRouteConfigs(routes),
  mTimeToSwitch(DEFAULT_TIME_TO_SWITCH),
  mTimer(0) {}

/* workaround hack to make sure servos get attached */
void Switch::initServo() {
  mServo.attach(mServoPin);
}

vector<RelaySwitchPosition>& Switch::getRelayPositions() {
  return mRelayPositions;
}

uint8_t Switch::getId() const {
  return mSwitchId;
}

SwitchPosition Switch::getSwitchPosition() {
  uint8_t errorThresh = 5;
  uint8_t currPos = mServo.read();
  SwitchPosition possiblePositions[] = {PositionIrrelevant, PositionMain, PositionSecondary};
  for (SwitchPosition pos : possiblePositions) {
    if (abs(currPos - pos) < errorThresh) {
      return pos;
    }
  }
}

pair<StationId, StationId> Switch::getStationsForPosition(SwitchPosition swPos) const {
  for (auto routeConfig : mRouteConfigs) {
    if (routeConfig.mPosition == swPos)
    {
      return pair<StationId, StationId>(routeConfig.mRoute.mCurrent, routeConfig.mRoute.mNext);
    }
  }
}


void Switch::startTimer() {
  mTimer = millis();
}

bool Switch::timerExpired() {
  bool expired = (mTimer && (millis() - mTimer >= mTimeToSwitch));
  if (expired) mTimer = 0;
  return expired;
}

void Switch::moveSwitch(SwitchPosition newPos) {
  // for (uint8_t currPos = mServo.read(); abs(newPos - currPos) > 0; currPos = currPos + (newPos - currPos) / (abs(newPos - currPos))) {
  //   mServo.write(currPos);
  //   Serial.print("Switch ");
  //   Serial.print(mSwitchId);
  //   Serial.print(" moving to ");
  //   Serial.println(currPos);
  //   delay(20);
  // }
  mServo.write(newPos);
}

void Switch::pushRoute(Route r) {
  SwitchPosition newPosition;
  for (RouteConfig rConfig : mRouteConfigs) {
    if (rConfig.mRoute.isEqual(r)) {
      newPosition = rConfig.mPosition;
      break;
    }
  }

  if (newPosition != PositionIrrelevant)
    mPositionQueue.push(newPosition);

  if (!mPositionQueue.empty())
    moveSwitch(mPositionQueue.front());
}

void Switch::popRoute() {
  if (!mPositionQueue.empty())
    mPositionQueue.pop();
  if (!mPositionQueue.empty())
    moveSwitch(mPositionQueue.front());
}
