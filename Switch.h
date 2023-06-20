#ifndef SWITCH_H
#define SWITCH_H
#include <queue>
#include <ESP32Servo.h>
#include "Station.h"
using namespace std;

#define DEFAULT_TIME_TO_SWITCH 1500

enum SwitchPosition {
  PositionIrrelevant = 0,
  PositionSecondary = 25,
  PositionMain = 90
};

struct Route {
  StationId mCurrent;
  StationId mNext;

  bool isEqual(const Route&);       
};

struct RelaySwitchPosition {
  Button mRelay;
  SwitchPosition mPosition;
};

struct RouteConfig {
  Route mRoute;
  SwitchPosition mPosition; 
};

class Switch {
private:
  const uint8_t mSwitchId;
  uint8_t mServoPin;
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

  void initServo();

  vector<RelaySwitchPosition>& getRelayPositions();
  uint8_t getId() const;
  SwitchPosition getSwitchPosition();
  pair<StationId, StationId> getStationsForPosition(SwitchPosition) const;

  void startTimer();
  bool timerExpired();

  void pushRoute(Route);
  void popRoute();
};


#endif