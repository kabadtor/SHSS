/*
  Button

  Turns on and off a light emitting diode(LED) connected to digital pin 13,
  when pressing a pushbutton attached to pin 2.

  The circuit:
  - LED attached from pin 13 to ground
  - pushbutton attached to pin 2 from +5V
  - 10K resistor attached to pin 2 from ground

  - Note: on most Arduinos there is already an LED on the board
    attached to pin 13.

  created 2005
  by DojoDave <http://www.0j0.org>
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Button
*/

#include <Wire.h>
#include "rgb_lcd.h"
#include "DoorSwitchHandler.h"
#include "FPS_GT511C3.h"


enum SystemState
{
    Alarm,
    Idle,
    ScanPrint,
    Enroll
};

struct StateHandler
{
  StateHandler()
    :buzzerDisarmed(false), validPrintScanned(false), doorOpened(false)
    {}
  bool buzzerDisarmed;
  bool validPrintScanned;
  bool doorOpened;
};

rgb_lcd lcd;

const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 2;     // the number of the pushbutton pin
const int doorSwitchPin = 5;
const int buzzerPin = 7;

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

DoorSwitchHandler SwitchHandler(doorSwitchPin, buttonPin);

StateHandler stateHandler = StateHandler();

 FPS_GT511C3 fps(8,9);
byte alert = 0x0;

void setup() {
  Serial.begin(115200);
  Wire.begin(0x08);
  pinMode(13, OUTPUT);
  Wire.onRequest(sendEvent);
  lcd.begin(16,2);
  lcd.setRGB(colorR, colorG, colorB);
  lcd.print("Idle Mode");
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  pinMode(doorSwitchPin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  delay(1000);
   fps.Open();
   fps.SetLED(true);
}

void sendEvent(int h)
{
  Wire.write(alert);
}

int buttonCt = 0;
bool buttonLocked = false;
long int buttonLastPressed = 0;
SystemState state = SystemState::Idle;

void signalDoorOpened()
{
  stateHandler.doorOpened = true;
}

void reset()
{
    buttonCt = 0;
    buttonLocked = false;
}

void handleIdleState(int buttonState)
{
  if (shouldSoundAlarm())
  {
    signalDoorOpened();
    state = SystemState::Alarm;
    reset();
    return;
  }
  lcd.setCursor(0,0);
  if (buttonState == HIGH) 
  {
    // turn LED on:
    if (!buttonLocked)
    {
        if (buttonCt)
        {
            // This is the second button press. Enter "Register New Fingerprint" Mode
            lcd.clear();
            delay(1000);
            lcd.print("Register New");
            lcd.setCursor(0,1);
            lcd.print("Fingerprint Mode");
            state = SystemState::Enroll;
            reset();
        }
        else
        {
            buttonCt++;
            buttonLocked = true;
            buttonLastPressed = millis();
        }      
    }
  } 
  else if (buttonCt && millis() - buttonLastPressed > 1000)
  {
    // Only one button press was issued. Enter "Scan Fingerprint" Mode
    lcd.clear();
    delay(1000);
    lcd.print("Scan Fingerprint");
    lcd.setCursor(0,1);
    lcd.print("Mode");
    state = SystemState::ScanPrint;
    reset();
  }
  else
  {
      buttonLocked = false;
  }
}

void handleEnrollState(int buttonState)
{
  if (shouldSoundAlarm())
  {
    signalDoorOpened();
    state = SystemState::Alarm;
    return;
  }
  bool enrolled = false;
  while(!enrolled)
  {
    if (buttonState == HIGH)
    {
        break;
    }
    enrolled = enrollFingerprint();
  }
  resetToIdle();
}

void handleScanPrintState(int buttonState)
{
  if (shouldSoundAlarm())
  {
    signalDoorOpened();
    state = SystemState::Alarm;
    return;
  }
  bool switchedModes = scanStateHandleDoorInput();
  if (!switchedModes)
  {
    bool success = scanValidFingerprint(buttonState);
    if (success)
    {
      disarmBuzzer();
    }
  }
}

bool scanStateHandleDoorInput()
{
  bool result = false;
  int doorInput = digitalRead(doorSwitchPin);
  if (doorInput)
  {
    stateHandler.doorOpened = true;
  }
  else if (!doorInput && stateHandler.doorOpened)
  {
    resetToIdle();
    result = true;
  }
  return result;
}

bool enrollFingerprint()
{
  lcd.clear();
  lcd.print("Beginning");
  lcd.setCursor(0,1);
  lcd.print("Enrollment...");
  delay(200);
	int enrollid = 0;
	bool usedid = true;
	while (usedid == true)
	{
		usedid = fps.CheckEnrolled(enrollid);
		if (usedid==true) enrollid++;
	}
	fps.EnrollStart(enrollid);

	// enroll
  lcd.setCursor(0,0);
  lcd.clear();
	lcd.print("Press finger");
	Serial.println(enrollid);
	while(fps.IsPressFinger() == false) delay(100);
	bool bret = false;
	do
  {
    bret = fps.CaptureFinger(true);
  }
	while(!bret);
  lcd.clear();
  lcd.print("Remove finger");
  fps.Enroll1();
  while(fps.IsPressFinger() == true) delay(100);
  lcd.clear();
  lcd.print("Press same");
  lcd.setCursor(0,1);
  lcd.print("finger yet again");
  while(fps.IsPressFinger() == false) delay(100);
  bret = false;
  do
  {
    bret = fps.CaptureFinger(true);
  }
  while(!bret);
  lcd.clear();
  lcd.print("Remove finger");
  fps.Enroll2();
  while(fps.IsPressFinger() == true) delay(100);
  lcd.clear();
  lcd.print("Press same");
  lcd.setCursor(0,1);
  lcd.print("finger yet again");
  while(fps.IsPressFinger() == false) delay(100);
  bret = false;
  do
  {
    bret = fps.CaptureFinger(true);
  }
  while(!bret);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Remove finger");
  int iret = fps.Enroll3();
  lcd.clear();
  if (!iret)
  {
    lcd.print("Enrollment");
    lcd.setCursor(0,1);
    lcd.print("Successful");
  }
  else
  {
    lcd.print("Enrollment failed");
    lcd.setCursor(0,1);
    lcd.print("Retrying...");
  }
  delay(200);
  return !iret;
}

bool scanValidFingerprint(int buttonState)
{
  bool success = false;
  Serial.print("Entered scanValidFingerprint. buttonState = ");
  // Serial.println(buttonState);
  //   if (buttonState == HIGH)
  //   {
  //       disarmBuzzer();
  //   }
  if (fps.IsPressFinger())
	{
    lcd.setCursor(0,0);
    lcd.clear();
		fps.CaptureFinger(false);
		int id = fps.Identify1_N();
		
	     /*Note:  GT-521F52 can hold 3000 fingerprint templates
                GT-521F32 can hold 200 fingerprint templates
                 GT-511C3 can hold 200 fingerprint templates. 
		            GT-511C1R can hold 20 fingerprint templates.
			 Make sure to change the id depending on what
			 model you are using */
		if (id <200) //<- change id value depending model you are using
		{//if the fingerprint matches, provide the matching template ID
			lcd.print("Verified ID: ");
			lcd.print(id);
      stateHandler.validPrintScanned = true;
      success = true;
		}
		else
		{//if unable to recognize
			lcd.print("Finger not found.");
		}
	}
	else
	{
		lcd.print("Scan valid print.");
	}
  return success;
}

void handleAlarmState(int buttonState)
{
  if (!alert)
    alert = 0x1;
  if (shouldSoundAlarm())
  {
      lcd.setCursor(0,0);
      lcd.print("INTRUDER ALERT!!!");
      lcd.setCursor(0,1);
      lcd.print("ENTER VALID PRINT");
      digitalWrite(buzzerPin, HIGH);
      scanValidFingerprint(buttonState);
  }
  else
  {
    disarmBuzzer();
    if (!digitalRead(doorSwitchPin))
    {
      resetToIdle();
    }
  }
}

void resetToIdle()
{
  Serial.println("### RESETTING TO IDLE MODE");
  stateHandler.doorOpened = false;
  stateHandler.validPrintScanned = false;
  state = SystemState::Idle;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Idle Mode");
  alert = 0x0;
}

void disarmBuzzer()
{
  stateHandler.validPrintScanned = true;
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(buzzerPin, LOW);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("System Disarmed.");
  lcd.setCursor(0,1);
  lcd.print("Please Enter.");
}

bool shouldSoundAlarm()
{
  Serial.print("shouldSoundAlarm: ");
  Serial.print("valid print scanned: ");
  Serial.print(!stateHandler.validPrintScanned);
  Serial.print(" and door is open: ");
  Serial.println(digitalRead(doorSwitchPin));
  return !stateHandler.validPrintScanned && (digitalRead(doorSwitchPin) || stateHandler.doorOpened);
}

void loop() {
  lcd.setCursor(0,0);
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  Serial.print("Current state: ");
  Serial.println(state);
  switch(state)
  {
    case SystemState::Alarm:
        handleAlarmState(buttonState);
        break;
    case SystemState::Idle:
        handleIdleState(buttonState);
        break;
    case SystemState::ScanPrint:
        handleScanPrintState(buttonState);
        break;
    case SystemState::Enroll:
        handleEnrollState(buttonState);
        break;
  }

}
