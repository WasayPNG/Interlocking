#ifndef BUTTON_H
#define BUTTON_H

struct Button {
  uint8_t mPin;
  uint64_t mDebounceTimer;
  uint64_t mDebounceDelay;
  bool mButtonState;
  bool mLastButtonState;

  Button(uint8_t pin) : mPin(pin), mDebounceTimer(0), mDebounceDelay(300), mButtonState(HIGH), mLastButtonState(HIGH) {
    pinMode(pin, INPUT_PULLUP);
  }
  bool isPressed() {
    bool readState = digitalRead(mPin);
    if (readState != mLastButtonState) {
      mDebounceTimer = millis();
    }

    if (millis() - mDebounceTimer > mDebounceDelay) {
      if (readState != mButtonState) {
        mButtonState = readState;
        Serial.println("BUTTON PRESSED!");
      }
    }
    
    mLastButtonState = readState;
    return !mButtonState;
  }
};

#endif