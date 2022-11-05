#include "Arduino.h"
#include <avr/sleep.h>//going to sleep

#ifndef _ARRAY_H
#define _ARRAY_H
#include "array.h"
#endif

#ifndef _OWNSD_H
#define _OWNSD_H
#include "ownsd.h"
#endif 

#ifndef _PIN_H
#define _PIN_H
#include <pin.h> //Custom Pin Library https://github.com/leo3oel/pin
#endif

#ifndef ARRAYSIZES
#define ARRAYSIZE 60
#define STRINGLENGTH 12
#endif

#include <LiquidCrystal.h>

//externe Variablen
extern LiquidCrystal lcd;
extern DigitalPin LCDled;

void sendToSleep(char names[][STRINGLENGTH], unsigned short striche[]);
void wakeUpAgain();