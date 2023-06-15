#ifndef SWITCH_H
#define SWITCH_H
#include <queue>
#include <ESP32Servo.h>
#include "Station.h"
using namespace std;

enum SwitchPosition {
  PositionIrrelevant = 0,
  PositionSecondary = 45,
  PositionMain = 90
};

struct Route {
  StationId mCurrent;
  StationId mNext;

  bool isEqual(const Route&);       
};

struct RelaySwitchPosition {
  uint8_t mPin;
  SwitchPosition mPosition;
};

struct RouteConfig {
  Route mRoute;
  SwitchPosition mPosition; 
};

class Switch {
private:
  const uint8_t mSwitchId;
  Servo mServo;
  std::vector<RelaySwitchPosition> mRelayPositions;
  queue<SwitchPosition> mPositionQueue;
  vector<RouteConfig> mRouteConfigs;
  uint64_t mTimeToSwitch;
  uint64_t mTimer;


public:
  static uint8_t SwitchCounter;
  void moveSwitch(SwitchPosition);
  Switch(uint8_t, vector<RelaySwitchPosition>, vector<RouteConfig>);

  vector<RelaySwitchPosition> getRelayPositions() const;
  uint8_t getId() const;
  SwitchPosition getSwitchPosition();
  StationId getStationForPosition(SwitchPosition) const;

  void startTimer();
  bool timerExpired() const;

  void pushRoute(Route);
  void popRoute();
};


#endif