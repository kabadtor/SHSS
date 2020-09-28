#include "DoorSwitchHandler.h"

DoorSwitchHandler::DoorSwitchHandler(int doorSwitchPinNum, int buzzerPinNum)
    :doorSwitchPin(doorSwitchPinNum), 
     buzzerPin(buzzerPinNum), 
     enteredValidFingerprint(false),
     soundAlarm(false)
    {}

void DoorSwitchHandler::HandleDoorInput()
{
    if (soundAlarm)
    {
        digitalWrite(buzzerPin, HIGH);
        return;
    }
    int isOpened = digitalRead(doorSwitchPin);
    if (isOpened)
    {
        if (!enteredValidFingerprint)
        {
            soundAlarm = true;
        }
    }
}

void DoorSwitchHandler::DisarmBuzzer()
{
    enteredValidFingerprint = true;
    soundAlarm = false;
    digitalWrite(buzzerPin, LOW);
}

void DoorSwitchHandler::ResetBuzzerState()
{
    enteredValidFingerprint = false;
    soundAlarm = false;
    digitalWrite(buzzerPin, LOW);
}
