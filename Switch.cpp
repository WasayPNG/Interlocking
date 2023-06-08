#include "Switch.h"

bool Route::isEqual(const Route &r) {
  return (mCurrent == r.mCurrent && mNext == r.mNext);
}

uint8_t Switch::SwitchCounter = 0;

Switch::Switch(uint8_t pin, vector<RouteConfig> routes) :
  mServo(), 
  mSwitchId(Switch::SwitchCounter++),
  mPositionQueue(),
  mRouteConfigs(routes) {
    mServo.attach(pin);
    moveSwitch(PositionMain);
}

uint8_t Switch::getId() const {
  return mSwitchId;
}

void Switch::moveSwitch(SwitchPosition newPos) {
  for (uint8_t currPos = mServo.read(); abs(newPos - currPos) > 0; currPos = currPos + (newPos - currPos) / (abs(newPos - currPos))) {
    mServo.write(currPos);
    delay(20);
  }
}

void Switch::queueRoute(Route r) {
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

