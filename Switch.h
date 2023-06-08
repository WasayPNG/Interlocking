#ifndef SWITCH_H
#define SWITCH_H
#include <queue>
#include <ESP32Servo.h>
#include "Station.h"
using namespace std;

enum SwitchPosition {
  PositionIrrelevant = 0,
  PositionMain = 90,
  PositionSecondary = 120
};

struct Route {
  StationId mCurrent;
  StationId mNext;

  bool isEqual(const Route&);       
};

struct RouteConfig {
  Route mRoute;
  SwitchPosition mPosition; 
};

class Switch {
private:
  const uint8_t mSwitchId;
  Servo mServo;
  queue<SwitchPosition> mPositionQueue;
  vector<RouteConfig> mRouteConfigs;

  void moveSwitch(SwitchPosition);

public:
  static uint8_t SwitchCounter;
  Switch(uint8_t, vector<RouteConfig>);

  uint8_t getId() const;

  void queueRoute(Route);
};


#endif