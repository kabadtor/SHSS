#include "Arduino.h"
#include "Wire.h"

#ifndef DOORSWITCHHANDLER_H
#define DOORSWITCHHANDLER_H
class DoorSwitchHandler
{
public:
  DoorSwitchHandler(int doorSwitchPinNumber, int buzzerPinNumber);

  void HandleDoorInput();
  void DisarmBuzzer();
  void ResetBuzzerState();

  int doorSwitchPin;
  int buzzerPin;
  bool enteredValidFingerprint;
  // bool openedDoor;
  bool soundAlarm;
};

#endif
