#ifndef BUTTON_H
#define BUTTON_H

#define DEFAULT_RELAY_DEBOUNCE 50

struct Button {
  uint8_t mPin;
  uint64_t mDebounceTimer;
  uint64_t mDebounceDelay;
  bool mButtonState;
  bool mLastButtonState;
  bool mWasPressed;

  Button(uint8_t pin, uint64_t debounceDelay = 300) : mPin(pin), mDebounceTimer(0), mDebounceDelay(debounceDelay), mButtonState(HIGH), mLastButtonState(HIGH), mWasPressed(HIGH) {
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
      }
    }
    
    mLastButtonState = readState;
    return !mButtonState;
  }

  bool justPressed() {
    bool beforeWasPressed = mWasPressed; 
    mWasPressed = mButtonState;
    return beforeWasPressed;
  }

  bool firstPress() {
    if (isPressed() && justPressed())
      return true;
    else if (!isPressed())
      justPressed();
    return false;
  }
};

#endif