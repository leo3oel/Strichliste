#include <LiquidCrystal.h>
#include "direction.h"
#include "sleep.h"

#ifndef _PIN_H
#define _PIN_H
#include <pin.h> //Custom Pin Library https://github.com/leo3oel/pin
#endif

#ifndef _OWNSD_H
#define _OWNSD_H
#include "ownsd.h"
#endif

#ifndef _ARRAY_H
#define _ARRAY_H
#include "array.h"
#endif

#ifndef ARRAYSIZES
#define ARRAYSIZE 60
#define STRINGLENGTH 12
#endif

#define interruptPin 2

// Variablen und Konstanten
const unsigned short lcdRS = 3, lcdEN = 4, lcdD4 = 5, lcdD5 = 6, lcdD6 = 7, lcdD7 = 8; // LCD Pins
LiquidCrystal lcd(lcdRS, lcdEN, lcdD4, lcdD5, lcdD6, lcdD7);

// Pins
Direction drehgeber(0, 1, 2);
DigitalPin StrichePlus(17, INPUT), StricheMinus(16, INPUT), StricheReset(15, INPUT);
DigitalPin Maintenance(14, INPUT);
DigitalPin LCDled(9, OUTPUT);
unsigned long inactivity = 0; // sleeptimer
const short disteeprom = 2;   // distance between persons in EEPROM

// Current Position in List
short PosInList = -1;    // Current position in List, -1 => display "make a selection"
short prevPosInList = 0; // Previous position in List

// Arrays for Striche/Names
unsigned short strichearray[ARRAYSIZE];   //= {6,0,12,0,1,0,0};//Striche
char namesarray[ARRAYSIZE][STRINGLENGTH] = {"Leonhard S", "Rainer S", "Sophie S", "Simon M"}; // Array mit all Names, max 12 Characters per Name
unsigned short poseeTOposlcd[ARRAYSIZE];  // Enthält an Position 0 Nummer von erstem Namen
short usedpers = 0;                       // Currently used Places in Array

void setup() // Setup
{
    lcd.begin(16, 2);
    LCDled.setOut(HIGH);
    // EEPROM lesen
    readEEPROM(strichearray); // Striche vom EEPROM lesen
    bool sdread = 1;          //= readSD(namesarray) also enable writesd; //read Names from SD
    if (sdread)               // check if reading was successful
    {
        // everything ok
    }
    else
    {

        lcd.println(F("SD-Karten Fehler"));
        delay(2000);
        LCDled.setOut(LOW); // turn off led
        while (true)
        {
            delay(100000); // do nothing
        }
    }
    usedpers = countarraylength(namesarray); // Anzahl der Benutzten stellen zählen
    sort(namesarray, poseeTOposlcd);         // poseeTOposlcd array sortieren
    // pinMode(PinMaintenance, INPUT);
    pinMode(interruptPin, INPUT); // Pin for wakeup
    lcd.print(F("Name:"));
    lcd.setCursor(8, 0);
    lcd.print(F("Striche:"));
    lcd.setCursor(0, 1);
    lcd.print(F("Bitte auswaehlen"));
}

void loop()
{
    // sleep inactivity abfrage nach gefühl
    if (inactivity > 20000) // Nach ca. 20s
    {
        inactivity = 0;
        PosInList = -1;
        sendToSleep(namesarray, strichearray);
        lcd.setCursor(0, 1);
        lcd.print(F("Bitte auswaehlen"));
    }

    // Hoch/runter schalten
    if (drehgeber.turnedClockwise()) // PosInList durchwählen positiv
    {
        inactivity = 0;
        PosInList++;
    }

    if (drehgeber.turnedCounterClockwise()) // PosInList durchwählen negativ
    {
        inactivity = 0;
        PosInList--;
    }

    if (PosInList >= usedpers) // Ende erreicht
        PosInList = -1;
    if (PosInList < -1)
        PosInList = usedpers - 1;
    if (PosInList != prevPosInList) // Neuen PosInList anzeigen
        schreiben(PosInList, poseeTOposlcd);

    // Striche hoch/runterzählen/zurücksetzen
    if (StrichePlus.posEDGE()) // Zugehörige Striche hochzählen
    {
        inactivity = 0;

        // EEPROM lesen
        strichearray[poseeTOposlcd[PosInList]] = EEPROM.read(poseeTOposlcd[PosInList] * disteeprom);
        strichearray[poseeTOposlcd[PosInList]]++;
        // EEPROM schreiben
        EEPROM.update((poseeTOposlcd[PosInList]) * disteeprom, strichearray[poseeTOposlcd[PosInList]]);
        schreiben(PosInList, poseeTOposlcd);
    }

    if (StricheMinus.posEDGE()) // Zugehörige Striche hochzählen
    {
        inactivity = 0;

        // EEPROM lesen
        strichearray[poseeTOposlcd[PosInList]] = EEPROM.read(poseeTOposlcd[PosInList] * disteeprom);
        if (strichearray[poseeTOposlcd[PosInList]] > 0) // Keine negativen Zahlen zulassen
        {
            strichearray[poseeTOposlcd[PosInList]]--;
            // EEPROM schreiben
            EEPROM.update((poseeTOposlcd[PosInList]) * disteeprom, strichearray[poseeTOposlcd[PosInList]]);
            schreiben(PosInList, poseeTOposlcd);
        }
    }

    if (StricheReset.posEDGE() && PosInList >= 0) // Striche zurücksetzen
    {
        inactivity = 0;
        bool reset = 0;
        short int i = 0;

        lcd.clear();
        lcd.setCursor(4, 0);
        lcd.print(F("ACHTUNG!"));
        lcd.setCursor(0, 1);
        lcd.print(strichearray[poseeTOposlcd[PosInList]]);
        lcd.setCursor(4, 1);
        lcd.print(F("EUR bezahlt?"));
        delay(1500); // 2sec delay
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print(F("Zum loeschen"));
        lcd.setCursor(0, 1);
        lcd.print(F("erneut druecken"));
       // 3 sec delay, danach erneute abfrage ob noch gedrückt
        while (!reset && i < 100) // Entweder reset oder 2 sec warten
        {
            reset = StricheReset.posEDGE();
            if (reset)
                strichearray[poseeTOposlcd[PosInList]] = 0;
            delay(20);
            i++;
        }
        schreiben(PosInList, poseeTOposlcd);
        EEPROM.update((poseeTOposlcd[PosInList]) * disteeprom, strichearray[poseeTOposlcd[PosInList]]);
    }

    prevPosInList = PosInList;
    inactivity++;
    delay(2); // 20ms delay
}