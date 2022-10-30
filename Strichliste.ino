#include <LiquidCrystal.h>
#include <avr/sleep.h>
//#include "functions.h"
#include<EEPROM.h>//Für dauerspeicher
//#include <RocketScream_LowPowerAVRZero.h>
#define ARRAYSIZE 60 //Anzahl der Personen festlegen
#define interruptPin 2
LiquidCrystal lcd(12, 11, 7, 8, 9, 10);

/*ToDo: 
 *  - möglichkeit zum posinlist neu anlegen einführen
 *    - zB Wartungsmodus:
 *      - Output auf SD Karte
 *      - In Reihenfolge wie aktuell sortiert
 *      - Format so wählen, dass leere Arrayzellen mit ausgegeben werden
 *      - posinlist in die leeren Arrayzellen schreiben
 *      - posinlist und Striche von SD Karte einlesen
 *      - direkt auf EEPROM speichern
 *    - vmtl eher: Eingabe per Serial
 *      - Eingabe nur vom Namen
 *      - Möglichkeit Personen aus Liste zu löschen
 */


//Variablen und Konstanten
const int switchPin = 3; // Striche hochzählen
const int nschalter = 2; //Namen hochzählen
const int resetP = 6; //Striche zurücksetzen
const int switchBackPin = 4;//Striche runterzählen
const int nschalterBackPin = 13;//Namen zurückschalten
const int maintenancePin = 5; //Wartungsmodus

unsigned long int inactivity=0; //sleeptimer
short int disteeprom=16;//abstand einzelner Personen im EEPROM
//const int wakeUpPin = 6;

//Striche hochzählen
bool switchState = 0;
bool prevSwitchState = 0;

//Namen hochzählen
bool nschalterstat = 0;
bool prevnschalterstat = 0;

//Listenposition
int posinlist = -1; //Zählt personen durch; -1 ist der bitte auswählen bildschirm
int posinlistprev=0;

//Striche zurücksetzen
bool resetS=0;
bool resetSprev=0;

//Striche runterzählen
bool switchStateBack = 0;
bool prevswitchStateBack = 0;

//Namen runterzählen
bool nschalterstatBack = 0;
bool prevnschalterstatBack =0;

//Wartungsmodus
bool maintenanceState = 0;
bool prevmaintenanceState = 0;

//Arrays für Striche/Namen
int striche[ARRAYSIZE]; //= {6,0,12,0,1,0,0};//Striche
char stnamen[ARRAYSIZE][12]; //= {"Leonhard S", "Sophie M", "Rainer S", "Georg S", "Simon S"};//Array mit allen Namen, länge auf 12 begrenzt
int usedpers=0; //Anzahl der belegten Plätze im Array, wird später gezählt


void schreiben (int);//Schreibt Namen/Striche auf LCD

void sort();//Sortieren der Arrays
void writeEEPROM();//Schreiben der kompletten Arrays
void sendToSleep();//Sleep Funktion
void wakeUpAgain();//Wakeup from Sleep

void setup() //Setup
{
  //EEPROM lesen
  readEEPROM();

  //countarraylength
  countarraylength();

  // Serial.print(usedpers);
  lcd.begin(16,2);
  pinMode(switchPin,INPUT);
  pinMode(nschalter,INPUT);
  pinMode(resetP,INPUT);
  pinMode(switchBackPin, INPUT);
  pinMode(nschalterBackPin, INPUT);
  pinMode(maintenancePin, INPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  lcd.print("Name:");
  lcd.setCursor(8,0);
  lcd.print("Striche:");
  lcd.setCursor(0,1);
  lcd.print("Bitte auswaehlen");
  sort();
}


void loop() {
    //sleep inactivity abfrage nach gefühl
    if(inactivity>1000)//Nach ca. 10s
    {
      inactivity = 0;
      posinlist=-1;
      //sendToSleep();
      lcd.setCursor(0,1);
      lcd.print("Bitte auswaehlen");
    }

    //Pins lesen
    nschalterstat = digitalRead(nschalter);
    switchState  = digitalRead(switchPin);
    resetS = digitalRead(resetP);
    switchStateBack = digitalRead(switchBackPin);
    nschalterstatBack = digitalRead(nschalterBackPin);
    maintenanceState = digitalRead(maintenancePin);
  
    {//Hoch/runter schalten
    if(!prevnschalterstat && nschalterstat)//posinlist durchwählen positiv
    {
      inactivity = 0;
      posinlist++;
    }
    
    if(!prevnschalterstatBack && nschalterstatBack)//posinlist durchwählen negativ
    {
      inactivity = 0;
      posinlist--;
    }}

    if(posinlist == -1)//Start
    {
      lcd.setCursor(0,1);
      lcd.print("Bitte auswaehlen");
    }

    if(posinlist>=usedpers)//Ende erreicht
      posinlist=-1;
    if(posinlist<-1)
      posinlist=usedpers-1;
    if(posinlist!=posinlistprev)//Neuen posinlist anzeigen
      schreiben(posinlist);

  //Striche hoch/runterzählen/zurücksetzen
    if(!prevSwitchState && switchState) //Zugehörige Striche hochzählen
    {
      inactivity = 0;
      
      //EEPROM lesen
      striche[posinlist] = EEPROM.read(posinlist*disteeprom);
      striche[posinlist]++;
      //EEPROM schreiben
      EEPROM.update((posinlist)*disteeprom,striche[posinlist]);
      schreiben(posinlist);
    }

    if(!prevswitchStateBack && switchStateBack) //Zugehörige Striche hochzählen
    {
      inactivity = 0;
      
      //EEPROM lesen
      striche[posinlist] = EEPROM.read(posinlist*disteeprom);
      if (striche[posinlist]>0)//Keine negativen Zahlen zulassen
      {
        striche[posinlist]--;
        //EEPROM schreiben
        EEPROM.update((posinlist)*disteeprom,striche[posinlist]);
        schreiben(posinlist);
      }
    }

    if(!resetSprev && resetS && posinlist>=0)//Striche zurücksetzen
    {
      inactivity = 0;
      
      resetSprev = resetS;
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("ACHTUNG!");
      lcd.setCursor(0,1);
      lcd.print(striche[posinlist]);
      lcd.setCursor(4,1);
      lcd.print("EUR bezahlt?");
      delay(2000);//2sec delay
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("zum loeschen");
      lcd.setCursor(0,1);
      lcd.print("gedrueckt halten");
      delay(3000);//3 sec delay, danach erneute abfrage ob noch gedrückt
      if(digitalRead(resetP))
      {
        striche[posinlist]=0;
      }
      schreiben(posinlist);
      EEPROM.update((posinlist)*disteeprom,striche[posinlist]);
    }
    }
    
    if(prevmaintenanceState ==0 && maintenanceState==1)
    {
      maintenance();
    }
    //Vorgängerzustände setzen
    prevnschalterstat = nschalterstat;
    posinlistprev = posinlist;
    prevSwitchState = switchState;
    resetSprev = resetS;
    prevswitchStateBack = switchStateBack;
    prevnschalterstatBack = nschalterstatBack;
    prevmaintenanceState = maintenanceState;
    inactivity++;
    delay(20);//20ms delay
}