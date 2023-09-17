/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-stepper-motor-28byj-48-uln2003/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  
  Based on Stepper Motor Control - one revolution by Tom Igoe
*/

/***************************************
 Designed for Arduino Nano ESP32 series 
***************************************/

#include <WiFi.h>
#include <WiFiUdp.h>
#include <Stepper.h>
#include <NTP.h>
#include "arduino_secrets.h"


const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution

// ULN2003 Motor Driver Pins
#define IN1_M D7
#define IN2_M D8
#define IN3_M D9
#define IN4_M D10
#define M_MIN A7  // magnete della lancetta dei minuti

#define IN1_H D2
#define IN2_H D3
#define IN3_H D4
#define IN4_H D5
#define M_ORE D6  // magnete della lancetta delle ore
volatile bool updateTime = false;
volatile bool updateAll = false;

bool toggle = false;

// initialize the stepper library
Stepper stepperM(stepsPerRevolution, IN1_M, IN3_M, IN2_M, IN4_M);  // Stepper della lancetta dei minuti

// initialize the stepper library
Stepper stepperH(stepsPerRevolution, IN1_H, IN3_H, IN2_H, IN4_H);  // Stepper della lancetta delle ore

WiFiUDP wifiUdp;
NTP ntp(wifiUdp);

hw_timer_t *Timer0_Cfg = NULL;
 
void IRAM_ATTR Timer0_ISR()
{
  if(ntp.hours() == 3 && ntp.minutes() == 0)
    updateAll = true;
  else
    updateTime = true;
}

void setup() {
  Serial.begin(115200);
  //while(!Serial);

  // set the speed at 5 rpm
  stepperM.setSpeed(5);
  stepperH.setSpeed(5);

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(IN1_M, OUTPUT);
  pinMode(IN2_M, OUTPUT);
  pinMode(IN3_M, OUTPUT);
  pinMode(IN4_M, OUTPUT);
  pinMode(M_MIN, INPUT);

  pinMode(IN1_H, OUTPUT);
  pinMode(IN2_H, OUTPUT);
  pinMode(IN3_H, OUTPUT);
  pinMode(IN4_H, OUTPUT);
  pinMode(M_ORE, INPUT);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting ...");
    delay(500);
    }
  
  Serial.println("Connected");  
  ntp.ruleDST("CEST", Last, Sun, Mar, 2, 120); // last sunday in march 2:00, timetone +120min (+1 GMT + 1h summertime offset)
  ntp.ruleSTD("CET", Last, Sun, Oct, 3, 60); // last sunday in october 3:00, timezone +60min (+1 GMT)
  ntp.begin();
  Serial.println("start NTP");

  resetHands();
  setTime();

  Timer0_Cfg = timerBegin(0, 80, true);
  timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
  timerAlarmWrite(Timer0_Cfg, 60000000, true);
  timerAlarmEnable(Timer0_Cfg);
}

bool alreadyDone = false;

void loop() {
  if (updateTime)
  {
    if(ntp.minutes() == 0 && !alreadyDone)
    {
      stepperH.step(172);
      alreadyDone = true;
    }

    if(alreadyDone && ntp.minutes() != 0)
      alreadyDone = false;

    stepperM.step(34.4);
    updateTime = false;
    Serial.println(ntp.formattedTime("%A %T")); // Www hh:mm:ss
  }

  if (updateAll)
  {
    // time to update all the hands and time
    // every day @3:00AM
    timerAlarmDisable(Timer0_Cfg);
  
    ntp.update();
    resetHands();
    setTime();
    updateAll = false;
  
    timerAlarmEnable(Timer0_Cfg);
    Serial.println(ntp.formattedTime("%A %T")); // Www hh:mm:ss
 }
    
}