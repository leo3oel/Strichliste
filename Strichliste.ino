#include <LiquidCrystal.h>
#include <avr/sleep.h>//going to sleep
#ifndef _PIN_H
#define _PIN_H
#include <EEPROM.h>
#include <pin.h> //Custom Pin Library https://github.com/leo3oel/pin
#endif
#include "direction.h"
#define interruptPin 2
#define ARRAYSIZE 55
#include "ownsd.h"

File MyFile;

//Variablen und Konstanten
 //set max number of people
const unsigned short int lcdRS = 3, lcdEN = 4, lcdD4 = 5, lcdD5 = 6, lcdD6 = 7, lcdD7 = 8; 
LiquidCrystal lcd(lcdRS, lcdEN, lcdD4, lcdD5, lcdD6, lcdD7);

//Pins
Direction drehgeber(0, 1, 2);
DigitalPin StrichePlus(17,INPUT), StricheMinus(16, INPUT), StricheReset(15, INPUT);
DigitalPin Maintenance(14, INPUT);
DigitalPin LCDled(9, OUTPUT);
unsigned long int inactivity=0; //sleeptimer
const short int disteeprom=16;//distance between persons in EEPROM

//Current Position in List
short int PosInList = -1; //Current position in List, -1 => display "make a selection"
short int prevPosInList = 0; //Previous position in List

//Arrays for Striche/Names
unsigned short int strichearray[ARRAYSIZE]; //= {6,0,12,0,1,0,0};//Striche
char namesarray[ARRAYSIZE][12]; //Array mit all Names, max 12 Characters per Name
short int usedpers=0; //Currently used Places in Array

bool clk, oldclk, dt, olddt;

void setup() //Setup
{
  //EEPROM lesen
  readEEPROM();

  //countarraylength
  countarraylength();
  lcd.begin(16,2);
  //pinMode(PinMaintenance, INPUT);
  pinMode(interruptPin, INPUT); //Pin for wakeup
  LCDled.setOut(HIGH);
  lcd.print(F("Name:"));
  lcd.setCursor(8,0);
  lcd.print(F("Striche:"));
  lcd.setCursor(0,1);
  lcd.print(F("Bitte auswaehlen"));
  sort();
}


void loop() {
    
  //sleep inactivity abfrage nach gefühl
  if(inactivity>20000)//Nach ca. 20s
  {
    inactivity = 0;
    PosInList=-1;
    sendToSleep();
    lcd.setCursor(0,1);
    lcd.print(F("Bitte auswaehlen"));
  }

  //Hoch/runter schalten
  if(drehgeber.turnedClockwise())//PosInList durchwählen positiv
  {
    inactivity = 0;
    PosInList++;
  }

  if(drehgeber.turnedCounterClockwise())//PosInList durchwählen negativ
  {
    inactivity = 0;
    PosInList--;
  }

  if(PosInList>=usedpers)//Ende erreicht
    PosInList=-1;
  if(PosInList<-1)
    PosInList=usedpers-1;
  if(PosInList!=prevPosInList)//Neuen PosInList anzeigen
    schreiben(PosInList);

  //Striche hoch/runterzählen/zurücksetzen
  if(StrichePlus.posEDGE()) //Zugehörige Striche hochzählen
  {
    inactivity = 0;
    
    //EEPROM lesen
    strichearray[PosInList] = EEPROM.read(PosInList*disteeprom);
    strichearray[PosInList]++;
    //EEPROM schreiben
    EEPROM.update((PosInList)*disteeprom,strichearray[PosInList]);
    schreiben(PosInList);
  }

  if(StricheMinus.posEDGE()) //Zugehörige Striche hochzählen
  {
    inactivity = 0;
    
    //EEPROM lesen
    strichearray[PosInList] = EEPROM.read(PosInList*disteeprom);
    if (strichearray[PosInList]>0)//Keine negativen Zahlen zulassen
    {
      strichearray[PosInList]--;
      //EEPROM schreiben
      EEPROM.update((PosInList)*disteeprom,strichearray[PosInList]);
      schreiben(PosInList);
    }
  }

  if(StricheReset.posEDGE() && PosInList>=0)//Striche zurücksetzen
  {
    inactivity = 0;
    bool reset = 0;
    short int i=0;

    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print(F("ACHTUNG!"));
    lcd.setCursor(0,1);
    lcd.print(strichearray[PosInList]);
    lcd.setCursor(4,1);
    lcd.print(F("EUR bezahlt?"));
    delay(1500);//2sec delay
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print(F("Zum loeschen"));
    lcd.setCursor(0,1);
    lcd.print(F("erneut druecken"));
    delay(1000);//3 sec delay, danach erneute abfrage ob noch gedrückt
    while(!reset && i<100) //Entweder reset oder 2 sec warten
    {
        reset = StricheReset.posEDGE();
        if(reset)
          strichearray[PosInList]=0;
        delay(20);
        i++;
    }
    schreiben(PosInList);
    EEPROM.update((PosInList)*disteeprom,strichearray[PosInList]);
  }


  if(Maintenance.posEDGE())
  {
    maintenance();
  }

  prevPosInList = PosInList;
  inactivity++;
  delay(2);//20ms delay
}