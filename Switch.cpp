#include "Switch.h"

bool Route::isEqual(const Route &r) {
  return (mCurrent == r.mCurrent && mNext == r.mNext);
}

uint8_t Switch::SwitchCounter = 0;

Switch::Switch(uint8_t servoPin, vector<RelaySwitchPosition> relayPins, vector<RouteConfig> routes) :
  mServo(),
  mRelayPositions(relayPins),
  mSwitchId(Switch::SwitchCounter++),
  mPositionQueue(),
  mRouteConfigs(routes),
  mTimeToSwitch(3000),
  mTimer(0) {
    Serial.println(servoPin);
    Serial.println("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa");
    mServo.attach(servoPin);
    for (auto rPinPair : relayPins)
    {
      pinMode(rPinPair.mPin, INPUT_PULLUP);
    }
}

vector<RelaySwitchPosition> Switch::getRelayPositions() const {
  return mRelayPositions;
}

uint8_t Switch::getId() const {
  return mSwitchId;
}

SwitchPosition Switch::getSwitchPosition() {
  /* TODO: has to change if degrees in SwitchPosition change */
  if (mServo.read() > PositionSecondary/2 && mServo.read() <= PositionSecondary) {
    return PositionSecondary;
  } else if (mServo.read() > PositionSecondary && mServo.read() <= PositionMain) {
    return PositionMain;
  }
  return PositionIrrelevant;
}

StationId Switch::getStationForPosition(SwitchPosition swPos) const {
  for (auto routeConfig : mRouteConfigs) {
    if (routeConfig.mPosition == swPos)
    {
      return routeConfig.mRoute.mCurrent;
    }
  }
}

void Switch::startTimer() {
  mTimer = millis();
}

bool Switch::timerExpired() const {
  return (mTimer >= mTimeToSwitch);
}

void Switch::moveSwitch(SwitchPosition newPos) {
  for (uint8_t currPos = mServo.read(); abs(newPos - currPos) > 0; currPos = currPos + (newPos - currPos) / (abs(newPos - currPos))) {
    mServo.write(currPos);
    Serial.print("Switch ");
    Serial.print(mSwitchId);
    Serial.print(" moving to ");
    Serial.println(currPos);
    delay(20);
  }
}

void Switch::pushRoute(Route r) {
  SwitchPosition newPosition;
  for (RouteConfig rConfig : mRouteConfigs) {
    if (rConfig.mRoute.isEqual(r)) {
      newPosition = rConfig.mPosition;
    }
  }

  if (newPosition != PositionIrrelevant)
    mPositionQueue.push(newPosition);

  if (!mPositionQueue.empty())
    moveSwitch(mPositionQueue.front());
}

void Switch::popRoute() {
  mPositionQueue.pop();
  if (!mPositionQueue.empty())
    moveSwitch(mPositionQueue.front());
}
