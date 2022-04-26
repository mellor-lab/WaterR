/*
  WaterR is an open source device to control and monitor fluid intake in experimental rodents.
  Initially developed by Riccardo Avvisati at the University of Bristol in 2020

  Released under GNU GPLv3 License.
*/

#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <SdFat.h>
#include <StepperMotor.h>
#include <EEPROM.h>
#include <RTClib.h>
#include <SPI.h>

RTC_DS3231 rtc;
SdFatSoftSpi<12, 11, 13> sd;

SdFile logfile;
char filename[50];

StepperMotor motor(31, 33, 35, 37);         //initialize motor on in1, in2, in3, in4 pins

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);    //initialize lcd screen (pins are fixed for the shield)

uint8_t volume;             //volume daily allowance in thents of ml (so that 1.5 ml is handled as 15) <- this is stored in EEPROM addres 4
uint8_t volume_;            //temp variable that is used to modify the volume value and overwrite it
float volumeLeft;           //variable to calculate volume left on allowance as floating point
float volumeDisplay;        //variable used to display volume as floating point for human reading
float volumeLeftDisplay;
float lastVolumeLeft = -1;

uint16_t deliveredUnits = 0;        //units of fluid delivered so far, initialize to zero
uint16_t deliveryUnits100ul = 7;    //coefficient used to convert volume to stepper motor steps based on syringe size
uint16_t lastAllowanceResetDay = 0;     //variable used to ensure that allowance is reset no more than once a day
uint16_t lastPrimingDay = 0;        //variable used to ensure that priming water delivery isn't given more than once a day
uint32_t lastWaterDeliveryTime = 0;

int8_t hourON;      //EEPROM address 0
int8_t minuteON;    //EEPROM address 1
int8_t hourOFF;     //EEPROM address 2
int8_t minuteOFF;   //EEPROM address 3
int8_t hourON_;     //temp variables for when the user is changing the timers
int8_t minuteON_;
int8_t hourOFF_;
int8_t minuteOFF_;

int8_t hourADJ;     //variables for manually adjusting the clock (too much drift, winter/summer time clock shift, etc.)
int8_t minuteADJ;
int8_t hourADJ_;
int8_t minuteADJ_;

uint8_t hourNOW;
uint8_t minuteNOW;
uint8_t dayNOW;
uint32_t unixtimeNOW;

uint16_t timestampNOW;    //timestamps
uint16_t timestampON;
uint16_t timestampOFF;

char volumeTem[5];        //char buffers for printf function
char timeONtem[10];
char timeOFFtem[10];
char hourONtem[3];
char minuteONtem[3];
char hourOFFtem[3];
char minuteOFFtem[3];
char piezoThreshTem[4];
char hourADJtem[3];
char minuteADJtem[3];
char timeADJtem[6];

uint8_t piezoPin = A8;      //pin connected to piezo sensor
uint8_t piezoGround = A9;   //pin that is pulled to ground to provide a "clean" ground for piezo sensor
int piezoThresh;            //threshold for lick detection, value is stored in eeprom
int piezoThresh_;           //temp var to hold user-modified piezo threshold value
uint32_t lastLickTime = 0;
uint32_t debounceLick = 1000;

uint8_t endSyringeStop = 25;
uint8_t endSyringeStopGround = 27;
uint8_t startSyringeStop = 100;

uint32_t lastInputTime = 0;
uint32_t debounce = 200;

uint16_t menuState = 0;
uint16_t lastMenuState = 1;

bool SDlogActive = false;
bool updateFlag = false;

//--------------------------------------------------------------- Functions

void stopMotor() {            //function that turns the motor off. Can be implemented if battery operation is required
  digitalWrite(31, LOW);
  digitalWrite(33, LOW);
  digitalWrite(35, LOW);
  digitalWrite(37, LOW);
}


int8_t readButtonPress() {                   //function that reads a button press based on A0 analog reading
  uint16_t buttonReading = analogRead(A0);

  if (buttonReading < 50) {
    return 4;
  }
  else if (buttonReading < 200) {
    return 1;
  }
  else if (buttonReading < 400) {
    return 2;
  }
  else if (buttonReading < 600) {
    return 3;
  }
  else if (buttonReading < 800 ) {
    return 0;
  }
  else {
    return -1;
  }
}


uint16_t readPiezoSignal() {
  uint16_t piezoRead = analogRead(piezoPin);

  //logfile.println(piezoRead);                 //debug lines, print the piezoRead value to SD
  //logfile.flush();                            //useful to check power supply noise on analog read

  return piezoRead;
}


void updateScreen() {                         //function that updates the screen when something changes (mostly because the user is navigating the menu and changing settings)
  if ((menuState != lastMenuState))  {

    switch (menuState) {
      case 0:
        lcd.clear();
        sprintf(timeONtem, "On  %02d:%02d", hourON, minuteON);
        sprintf(timeOFFtem, "Off %02d:%02d", hourOFF, minuteOFF);
        lcd.setCursor(0, 0);
        lcd.print(timeONtem);
        lcd.setCursor(0, 1);
        lcd.print(timeOFFtem);

        lcd.setCursor(13, 0);
        lcd.print("Vol");
        lcd.setCursor(13, 1);
        if (volume == 0) {
          lcd.print("N/A");
        }
        else {
          volumeLeftDisplay = float(volumeLeft) / 10;

          lcd.print(volumeLeftDisplay);
        }
        break;

      case 1:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Set Timer");
        break;
      case 2:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Set Volume");
        break;
      case 3:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Adjust Syringe");
        break;
      case 4:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Set piezo Threshold");
        break;
      case 5:
        { lcd.clear();
          DateTime now = rtc.now();
          sprintf(timeADJtem, "%02d:%02d", now.hour(), now.minute());
          lcd.setCursor(0, 0);
          lcd.print("Adjust Clock");
          lcd.setCursor(4, 1);
          lcd.print(timeADJtem);
          break;
        }
      case 11:
        if (lastMenuState == 1) {
          hourON_ = hourON;
          minuteON_ = minuteON;
          hourOFF_ = hourOFF;
          minuteOFF_ = minuteOFF;
        }

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Time ON");
        lcd.setCursor(0, 1);
        lcd.print("Time OFF");

        sprintf(hourONtem, "%02d", hourON_);
        sprintf(minuteONtem, "%02d", minuteON_);
        sprintf(hourOFFtem, "%02d", hourOFF_);
        sprintf(minuteOFFtem, "%02d", minuteOFF_);

        lcd.setCursor(10, 0);
        lcd.print(hourONtem);
        lcd.print(":");
        lcd.print(minuteONtem);
        lcd.setCursor(10, 1);
        lcd.print(hourOFFtem);
        lcd.print(":");
        lcd.print(minuteOFFtem);

        lcd.setCursor(9, 0);
        lcd.print(">");
        break;
      case 12:
        lcd.setCursor(9, 0);
        lcd.print(" ");
        lcd.setCursor(9, 1);
        lcd.print(" ");

        lcd.setCursor(15, 0);
        lcd.print("<");
        break;
      case 13:
        lcd.setCursor(15, 0);
        lcd.print(" ");
        lcd.setCursor(15, 1);
        lcd.print(" ");

        lcd.setCursor(9, 1);
        lcd.print(">");
        break;
      case 14:
        lcd.setCursor(9, 1);
        lcd.print(" ");

        lcd.setCursor(15, 1);
        lcd.print("<");
        break;

      case 31:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Move Syringe w/");
        lcd.setCursor(0, 1);
        lcd.print("Up/Down buttons");
        break;

      case 21:
        volume_ = volume;
        volumeDisplay = float(volume_) / 10;

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Volume: ");
        lcd.setCursor(8, 0);
        lcd.print(volumeDisplay);
        lcd.setCursor(13, 0);
        lcd.print("ml");
        break;

      case 41:
        piezoThresh_ = piezoThresh;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Thresh for lick:");
        lcd.setCursor(0, 1);
        lcd.print(piezoThresh_);
        break;

      case 51:
        if (lastMenuState == 5) {
          DateTime now = rtc.now();
          hourADJ_ = now.hour();
          minuteADJ_ = now.minute();
        }
        sprintf(hourADJtem, "%02d", hourADJ_);
        sprintf(minuteADJtem, "%02d", minuteADJ_);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Set current time");
        lcd.setCursor(4, 1);
        lcd.print(hourADJtem);
        lcd.print(":");
        lcd.print(minuteADJtem);

        lcd.setCursor(3, 1);
        lcd.print(">");
        break;

      case 52:
        lcd.setCursor(3, 1);
        lcd.print(" ");
        lcd.setCursor(9, 1);
        lcd.print("<");
        break;
    }
    lastMenuState = menuState;
  }
  else if (updateFlag) {
    switch (menuState) {
      case 11:
        sprintf(hourONtem, "%02d", hourON_);
        lcd.setCursor(10, 0);
        lcd.print(hourONtem);
        break;
      case 12:
        sprintf(minuteONtem, "%02d", minuteON_);
        lcd.setCursor(13, 0);
        lcd.print(minuteONtem);
        break;
      case 13:
        sprintf(hourOFFtem, "%02d", hourOFF_);
        lcd.setCursor(10, 1);
        lcd.print(hourOFFtem);
        break;
      case 14:
        sprintf(minuteOFFtem, "%02d", minuteOFF_);
        lcd.setCursor(13, 1);
        lcd.print(minuteOFFtem);
        break;

      case 21:
        volumeDisplay = float(volume_) / 10;
        lcd.setCursor(8, 0);
        lcd.print(volumeDisplay);
        break;

      case 41:
        sprintf(piezoThreshTem, "%3d", piezoThresh_);
        lcd.setCursor(0, 1);
        lcd.print(piezoThreshTem);
        break;

      case 51:
        sprintf(hourADJtem, "%02d", hourADJ_);
        lcd.setCursor(4, 1);
        lcd.print(hourADJtem);
        break;

      case 52:
        sprintf(minuteADJtem, "%02d", minuteADJ_);
        lcd.setCursor(7, 1);
        lcd.print(minuteADJtem);
        break;

      case 0:
        volumeLeftDisplay = float(volumeLeft) / 10;
        lcd.setCursor(13, 1);
        if ((volume != 0) && (volumeLeft >= 0)) {
          lcd.print(volumeLeftDisplay);
        }
        else if (volume == 0) {
          lcd.print("N/A");
        }
        break;
    }
    updateFlag = false;
  }
}


void mapCommand(uint8_t button) {           //function that triggers a command depending on button pressed and current menu state

  if ((button == 1) && (millis() - lastInputTime >= debounce)) {                      //Up button
    switch (menuState) {
      case 0:
        menuState = 5;
        break;
      case 1:
        menuState = 0;
        break;
      case 2:
        menuState = 1;
        break;
      case 3:
        menuState = 2;
        break;
      case 4:
        menuState = 3;
        break;
      case 5:
        menuState = 4;
        break;

      case 31:
        motor.setStepDuration(1);
        while ((analogRead(A0) < 200)) { // (digitalRead(endSyringeStop) == HIGH)) {
          motor.step(8);
        }
        stopMotor();
        motor.setStepDuration(50);
        break;

      case 11:
        hourON_++;
        if (hourON_ >= 24) {
          hourON_ = 0;
        }
        updateFlag = true;
        break;
      case 12:
        minuteON_++;
        if (minuteON_ >= 60) {
          minuteON_ = 0;
        }
        updateFlag = true;
        break;
      case 13:
        hourOFF_++;
        if (hourOFF_ >= 24) {
          hourOFF_ = 0;
        }
        updateFlag = true;
        break;
      case 14:
        minuteOFF_++;
        if (minuteOFF_ >= 60) {
          minuteOFF_ = 0;
        }
        updateFlag = true;
        break;

      case 21:
        volume_ += 1;
        if (volume_ > 99) {
          volume_ = 0;
        }
        updateFlag = true;
        break;

      case 41:
        piezoThresh_ += 5;
        if (piezoThresh_ > 990) {
          piezoThresh_ = 20;
        }
        updateFlag = true;
        break;

      case 51:
        hourADJ_++;
        if (hourADJ_ >= 24) {
          hourADJ_ = 0;
        }
        updateFlag = true;
        break;
      case 52:
        minuteADJ_++;
        if (minuteADJ_ >= 60) {
          minuteADJ_ = 0;
        }
        updateFlag = true;
        break;
    }
    lastInputTime = millis();
  }

  if ((button == 2) && (millis() - lastInputTime >= debounce)) {              //Down button
    switch (menuState) {
      case 0:
        menuState = 1;
        break;
      case 1:
        menuState = 2;
        break;
      case 2:
        menuState = 3;
        break;
      case 3:
        menuState = 4;
        break;
      case 4:
        menuState = 5;
        break;
      case 5:
        menuState = 0;
        break;

      case 31:
        motor.setStepDuration(1);
        while ((analogRead(A0) < 400)) {//  && (digitalRead(endSyringeStop) == HIGH)) {
          motor.step(-8);
        }
        stopMotor();
        motor.setStepDuration(50);
        break;

      case 11:
        hourON_--;
        if (hourON_ < 0) {
          hourON_ = 23;
        }
        updateFlag = true;
        break;
      case 12:
        minuteON_--;
        if (minuteON_ < 0) {
          minuteON_ = 59;
        }
        updateFlag = true;
        break;
      case 13:
        hourOFF_--;
        if (hourOFF_ < 0) {
          hourOFF_ = 23;
        }
        updateFlag = true;
        break;
      case 14:
        minuteOFF_--;
        if (minuteOFF_ < 0) {
          minuteOFF_ = 59;
        }
        updateFlag = true;
        break;

      case 21:
        volume_ -= 1;
        if (volume_ < 0) {
          volume_ = 99;
        }
        updateFlag = true;
        break;

      case 41:
        piezoThresh_ -= 5;
        if (piezoThresh_ < 20) {
          piezoThresh_ = 990;
        }
        updateFlag = true;
        break;

      case 51:
        hourADJ_--;
        if (hourADJ_ < 0) {
          hourADJ_ = 23;
        }
        updateFlag = true;
        break;
      case 52:
        minuteADJ_--;
        if (minuteADJ_ < 0) {
          minuteADJ_ = 59;
        }
        updateFlag = true;
        break;
    }
    lastInputTime = millis();
  }

  if (((button == 4) || (button == 0)) && (millis() - lastInputTime >= debounce)) {          //Right or Select button
    switch (menuState) {
      case 1:
        menuState = 11;
        break;
      case 2:
        menuState = 21;
        break;
      case 3:
        menuState = 31;
        break;
      case 4:
        menuState = 41;
        break;
      case 5:
        menuState = 51;
        break;

      case 11:
        menuState = 12;
        break;
      case 12:
        menuState = 13;
        break;
      case 13:
        menuState = 14;
        break;
      case 14:
        hourON = hourON_;
        minuteON = minuteON_;
        hourOFF = hourOFF_;
        minuteOFF = minuteOFF_;
        EEPROM.write(0, hourON);
        EEPROM.write(1, minuteON);
        EEPROM.write(2, hourOFF);
        EEPROM.write(3, minuteOFF);
        menuState = 0;
        lastAllowanceResetDay = 0;                //if timer is changed, zero the reset day so that allowance will reset when the new timer is reached 
        EEPROM.write(9, lastAllowanceResetDay);
        lastPrimingDay = 0;                       //if timer is changed, zero priming day so that a priming will be delivered when the new timer is reached
        EEPROM.write(5, lastPrimingDay);
        break;

      case 21:
        volume = volume_;
        EEPROM.write(4, volume);
        volumeLeft = volume;
        deliveredUnits = 0;
        menuState = 0;
        break;

      case 31:
        menuState = 3;
        break;

      case 41:
        { piezoThresh = piezoThresh_;
          byte byte1write = piezoThresh >> 8;
          byte byte2write = piezoThresh & 0xff;
          EEPROM.write(6, byte1write);
          EEPROM.write(7, byte2write);
          menuState = 0;
          break;
        }

      case 51:
        menuState = 52;
        break;
      case 52:
        { hourADJ = hourADJ_;
          minuteADJ = minuteADJ_;
          DateTime now = rtc.now();
          rtc.adjust(DateTime(now.year(), now.month(), now.day(), hourADJ_, minuteADJ_, 0));
          menuState = 0;
          break;
        }
    }
    lastInputTime = millis();
  }

  if ((button == 3) && (millis() - lastInputTime >= debounce)) {        //Left button
    switch (menuState) {
      case 11:
        menuState = 0;
        break;
      case 12:
        menuState = 11;
        break;
      case 13:
        menuState = 12;
        break;
      case 14:
        menuState = 13;
        break;

      case 21:
        menuState = 2;
        break;

      case 31:
        menuState = 3;
        break;

      case 51:
        menuState = 5;
        break;
      case 52:
        menuState = 51;
        break;
    }
    lastInputTime = millis();
  }

  if ((menuState != 0 ) && (millis() - lastInputTime >= 10000)) {
    menuState = 0;
  }
}


bool gateIsOpen() {
  if ((volume == 0) && (timestampON == 0) && (timestampOFF == 0)) {             //if volume and timestamps are set to 0
    return true;                                                                //always true: ad libitum consumption
  }
  else if ((volume == 0) && (timestampON != timestampOFF)) {                    //if volume is 0 but timestamps are different from each other
    if ((timestampNOW >= timestampON) && (timestampNOW < timestampOFF)) {       //return true if "now" is inside period specified by timestamps
      return true;                                                              //fixed time availability restriction
    }
    else {
      return false;
    }
  }
  else if ((volume != 0) && (timestampON == timestampOFF)) {                    //if volume is different from 0 but timestamps are the same
    if (volumeLeft > 0) {                                                       //return true if there is still volume to be delivered
      return true;                                                              //fixed volume restriction
    }                                                                           
    else {
      return false;
    }
  }
  else if ((volume != 0) && (timestampON != timestampOFF)) {                                       //if volume is different from 0 and timestamps are different from each other
    if ((timestampNOW >= timestampON) && (timestampNOW < timestampOFF) && volumeLeft > 0) {        //hybrid mode: fixed volume within fixed time window
      return true;
    }
    else {
      return false;
    }
  }
}


void updateTimestamps() {                       //this function polls the rtc and generates timestamps for "now", ON and OFF timers
  DateTime now = rtc.now();

  unixtimeNOW = now.unixtime();

  hourNOW = now.hour();
  minuteNOW = now.minute();
  dayNOW = now.day();

  timestampNOW = hourNOW * 60 + minuteNOW;      //timestamps are more like "minutestamps", as in the minute number of the day (between 0 and 1440)
  timestampON = hourON * 60 + minuteON;         //this makes it easier to compare them in order to open/close water access
  timestampOFF = hourOFF * 60 + minuteOFF;
}


void checkAllowance() {                                                //function that controls daily allowance
  volumeLeft -= floor(deliveredUnits / deliveryUnits100ul);
  if (volumeLeft != lastVolumeLeft) {
    updateFlag = true;
    lastVolumeLeft = volumeLeft;
    deliveredUnits = 0;
    EEPROM.write(8, volumeLeft);
  }

  if ((timestampNOW >= timestampON) && (dayNOW != lastAllowanceResetDay)) {    //if "now" is the time, reset the amount consumed so far
    volumeLeft = volume;                                                       //now.day returns the day number in the month, only reset if this value has changed since last reset (it is a different day)
    lastAllowanceResetDay = dayNOW;                                            //update lastAllowanceResetDay 
    EEPROM.write(9, lastAllowanceResetDay);
  }
}


void deliverPriming() {
  if ((dayNOW != lastPrimingDay) && (((timestampNOW >= timestampON) && (timestampNOW <= timestampOFF)) || (timestampON + timestampOFF == 0))) {
    uint16_t primingSteps = 8 * floor(deliveryUnits100ul * 0.8);
    motor.step(primingSteps);
    stopMotor();
    lastWaterDeliveryTime = millis();
    lastPrimingDay = dayNOW;
    EEPROM.write(5, lastPrimingDay);
    logfile.print(unixtimeNOW);
    logfile.println(",initial priming");
    logfile.flush();
  }
  if ((timestampON + timestampOFF != 0) && gateIsOpen() && (millis() - lastWaterDeliveryTime > 1800000)) {       //if water is available and WaterR is not in ad libitum mode (when timestampON and timestampOFF are both set to 0)
    uint16_t initializeSteps = 8 * floor(deliveryUnits100ul * 0.6);                                              //deliver a priming if the mouse hasn't drank in 30 minutes (keeps the spout wet so the mouse knows water is available)
    motor.step(initializeSteps);
    stopMotor();
    lastWaterDeliveryTime = millis();
    logfile.print(unixtimeNOW);
    logfile.println(",additional priming");
    logfile.flush();
  }
  else if ((timestampON + timestampOFF == 0) && (volume == 0) && (millis() - lastWaterDeliveryTime > 21600000)) {         //if waterR is in ad libitum mode
    uint16_t initializeSteps = 8 * floor(deliveryUnits100ul * 0.8);                                                       //deliver a priming if the mouse hasn't drank in 6 hours
    motor.step(initializeSteps);
    stopMotor();
    lastWaterDeliveryTime = millis();
    logfile.print(unixtimeNOW);
    logfile.println(",additional priming");
    logfile.flush();
  }
}


void setup() {
  rtc.begin();                    //initialize real time clock
  lcd.begin(16, 2);               //initialize lcd screen

  Serial.begin(115200);           //initialize Serial (for debugging)

  pinMode(A0, INPUT);             //set A0 as input for button read
  pinMode(piezoPin, INPUT);
  pinMode(piezoGround, OUTPUT);
  digitalWrite(piezoGround, LOW);

  pinMode(endSyringeStop, INPUT_PULLUP);

  hourON = EEPROM.read(0);        //retrieve hour and minute for ON and OFF timers from EEPROM memory
  minuteON = EEPROM.read(1);
  hourOFF = EEPROM.read(2);
  minuteOFF = EEPROM.read(3);

  byte byte1PiezoThresh = EEPROM.read(6);       //read the 2 bytes for piezoThresh stored in EEPROM
  byte byte2PiezoThresh = EEPROM.read(7);

  piezoThresh = (byte1PiezoThresh << 8) + byte2PiezoThresh;     //combine the 2 bytes to obtain the real, int value

  updateTimestamps();             //generate timestamps from values

  volume = EEPROM.read(4);
  volumeLeft = EEPROM.read(8);

  if (volume == 0) {        //if fixed volume is not active, overwrite lastPrimingDay EEPROM stored value so if device is power-cycled a priming is delivered even if it was already delivered during the day
    EEPROM.write(5, 0);
  }
  lastPrimingDay = EEPROM.read(5);

  lastAllowanceResetDay = EEPROM.read(9);
  
  pinMode(53, OUTPUT);        //set hardware SPI CS pin as output (even if we're using software SPI)
  if (sd.begin(10)) {         //initialize SD card on pin 10, if that's successful..
    DateTime now = rtc.now(); //poll RTC for time
    sprintf(filename, "WaterR_%04d-%02d-%02d_%02d-%02d-%02d.csv", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
    logfile.open(filename, O_WRITE | O_CREAT);    //open a file with date/time in the name (avoid accidental overwrites)
    if (sd.exists(filename)) {
      SDlogActive = true;
    }
  }
}

void loop() {

  int8_t button = readButtonPress();      //read the press of a button
  mapCommand(button);                     //implement button action depending on menuState
  updateScreen();                         //update the LCD as needed

  updateTimestamps();       //check current, on and off periods timestamps
  checkAllowance();         //check whether the daily consumption needs to be reset
  deliverPriming();         //delivers priming

  uint16_t piezoSignal = readPiezoSignal();

  if ((piezoSignal > piezoThresh) && (millis() - lastLickTime >= debounceLick)) {       //if spout is licked (debounced, maximum once a second)
    if (digitalRead(endSyringeStop) == LOW) {    //if the syringe is empty,
      delay(400);                                 //"dummy" water delivery (takes 50 ms per step, 50x8=400)

      logfile.print(unixtimeNOW);                 //log unixtime
      logfile.println(",lick EMPTY");             //log attempted lick and syringe empty event
      logfile.flush();                            //flush file to ensure data is written to SD
    }

    else if (gateIsOpen()) {            //if conditions for water delivery are met
      motor.step(8);                    //step the motor (8 steps = 1 cycle);
      stopMotor();

      logfile.print(unixtimeNOW);       //log unixtime
      logfile.println(",lick OK");      //log lick attemp with water delivery event
      logfile.flush();                  //flush file to ensure data is written to SD

      deliveredUnits++;                 //increment a counter that is used to calculate how big a volume has been consumed so far today
      lastWaterDeliveryTime = millis(); //note time of last water delivery (needed for primings to keep the spout wet if water has not been delivered in a while)
    }
    else if (!gateIsOpen()) {           //if conditions for water delivery are NOT met
      delay(400);                       //"dummy" water delivery (takes 50 ms per step, 50x8=400)

      logfile.print(unixtimeNOW);       //log unixtime
      logfile.println(",lick KO");      //log lick attemp without water delivery event
      logfile.flush();                  //flush file to ensure data is written to SD
    }
    lastLickTime = millis();
  }
}
