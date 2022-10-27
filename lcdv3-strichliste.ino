#include <LiquidCrystal.h>
#include <avr/sleep.h>
//#include "functions.h"
#include<EEPROM.h>//Für dauerspeicher
//#include <RocketScream_LowPowerAVRZero.h>
#define ARRAYSIZE 50 //Anzahl der Personen festlegen
#define interruptPin 2
LiquidCrystal lcd(12, 11, 5, 4, 3, 9);

/*Gedanken: 
 *  - möglichkeit zum posinlist neu anlegen einführen
 *    - zB Wartungsmodus:
 *      - Output auf SD Karte
 *      - In Reihenfolge wie aktuell sortiert
 *      - Format so wählen, dass leere Arrayzellen mit ausgegeben werden
 *      - posinlist in die leeren Arrayzellen schreiben
 *      - posinlist und Striche von SD Karte einlesen
 *      - direkt auf EEPROM speichern
 */


//Variablen und Konstanten
const int switchPin = 6; // Striche hochzählen
const int nschalter = 7; //Namen durchschalten
const int resetP = 8; //Striche zurücksetzen
unsigned long int inactivity=0; //sleeptimer
short int disteeprom=16;//abstand einzelner Personen im EEPROM
//const int wakeUpPin = 6;

//Striche hochzählen
int switchState = 0;
int prevSwitchState = 0;

//Namen hochzählen
int nschalterstat = 0;
int prevnschalterstat = 0;

//Listenposition
int posinlist = -1; //Zählt personen durch; -1 ist der bitte auswählen bildschirm
int posinlistprev=0;
short int disteeprom=16;//abstand einzelner Personen im EEPROM

//Striche zurücksetzen
int resetS=0;
int resetSprev=0;

//Arrays für Striche/Namen
int striche[ARRAYSIZE] = {6,0,12,0,1,0,0};//Striche
char stnamen[ARRAYSIZE][12] = {"Leonhard S", "Sophie M", "Rainer S", "Georg S", "Simon S"};//Array mit allen Namen, länge auf 12 begrenzt
int usedpers=0; //Anzahl der belegten Plätze im Array, wird später gezählt


void schreiben (int);//Schreibt Namen/Striche auf LCD
void zeileloeschen (int a)//Löscht Display
{
  for(int i=0; i<=16; i++)
  {
    lcd.setCursor(i,a);
    lcd.print(" ");
  }
}

void sort();//Sortieren der Arrays
void writeEEPROM();//Schreiben der kompletten Arrays
void sendToSleep();//Sleep Funktion
void wakeUpAgain();//Wakeup from Sleep

void setup() //Setup
{

  //!!!!!!!!!!!!!
  //Nur um neue Mitglieder anzulegen
  //Später bessere Lösung finden:
  
  //EEPROM schreiben
  //void  writeEEPROM();

  //countarraylength
  usedpers=0;
  for(int i=0; i<ARRAYSIZE; i++)
    if(stnamen[i][0]!=0)
      usedpers++;
   
  
  //EEPROM lesen !!!GEHT NICHT (kann auch problem beim schreiben sein)
/*  for(int i = 0; i<=usedpers; i++)
  {
    int ieeprom = i*disteeprom;
    striche[i] = EEPROM.read(ieeprom); //Striche aus EEPROM lesen
    for(int j = 0; j<=12; j++)
    {
      stnamen[i][j] = EEPROM.read(ieeprom+j); //Einzelne Character aus EEPROM in die Strings einlesen
    }    
  }*/
  
  lcd.begin(16,2);
  pinMode(switchPin,INPUT);
  pinMode(nschalter,INPUT);
  pinMode(resetP,INPUT);
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
  if(inactivity>100000)
  {
    inactivity = 0;
    posinlist=-1;
    sendToSleep();
    lcd.setCursor(0,1);
    lcd.print("Bitte auswaehlen");
  }
  
  //Pins lesen
  nschalterstat = digitalRead(nschalter);
  switchState  = digitalRead(switchPin);
  resetS = digitalRead(resetP);
  
  if(!prevnschalterstat && nschalterstat)//posinlist durchwählen
  {
    inactivity = 0;
    posinlist++;
  }
  
  if(posinlist == -1)//Start
  {
    lcd.setCursor(0,1);
    lcd.print("Bitte auswaehlen");
  }

  if(posinlist>=usedpers)//Ende erreicht
    posinlist=-1;
  if(posinlist!=posinlistprev)//Neuen posinlist anzeigen
    schreiben(posinlist);

    
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

  if(!resetSprev && resetS && posinlist>0)//Striche zurücksetzen
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
    delay(5000);//5 sec delay, danach erneute abfrage ob noch gedrückt
    if(digitalRead(resetP))
    {
      striche[posinlist]=0;
    }
    schreiben(posinlist);
    EEPROM.update((posinlist)*disteeprom,striche[posinlist]);
  }
  
  //Vorgängerzustände setzen
  prevnschalterstat = nschalterstat;
  posinlistprev = posinlist;
  prevSwitchState = switchState;
  resetSprev = resetS;

  inactivity++;
}

//-------------------------------------------------------------

void schreiben (int a)//funktion zum schreiben des lcd displays
{
  if(a!=-1)
  {
    lcd.clear();
    lcd.print("Name:");
    lcd.setCursor(8,0);
    lcd.print("Striche:");
    lcd.setCursor(0,1);
    lcd.print(stnamen[a]);
    lcd.setCursor(14,1); 
    lcd.print(striche[a]);
  }
}

void writeEEPROM()
{
    /*
     * Schreibt die kompletten Arrays auf das EEPROM
     * Später noch das schreiben der SD Karte hier ergänzen
     */ 
    for(int i=0; i<ARRAYSIZE; i++)
    {
      int ieeprom = i*disteeprom;
      EEPROM.write(ieeprom, striche[i]);//Striche schreiben
      for(int j=0; j<12; j++)
      {
        EEPROM.write(((ieeprom+1)+j),stnamen[i][j]);
        Serial.println(stnamen[i][j]);
      }
    }
}


void sort() //Sortiert und schreibt auf EEPROM
{
  /*
   * Sortiert Striche und Namen parallel im double Sort.
   * Die sortierten Arrays werden auf den EEPROM geschriben
   */
  char tempnamen[12];
  int tempstriche=0;
  for(int i=0; i<ARRAYSIZE; i++)
  {
    for(int j=0;j<ARRAYSIZE; j++)
      if(striche[j]<striche[j+1])
      {
        for(int c=0; c<12;c++)
        {
          //Namen tauschen
          tempnamen[c] = stnamen[j][c];
          stnamen[j][c] = stnamen[j+1][c];
          stnamen[j+1][c] = tempnamen[c];
        }

        //Striche tauschen
        tempstriche = striche[j];
        striche[j] = striche[j+1];
        striche[j+1] = tempstriche;
      }
  }
  writeEEPROM();
}

void sendToSleep()
{
  sort();
  //enable sleeping - note this primes sleep, not starts it!
  sleep_enable();
  //attach the interrupt, specify the pin, the method to call on interrupt,
  //and the interrupt conditions, in this case when the pin is pulled low.
  attachInterrupt(interruptPin, wakeUpAgain, LOW);
  //actually activate sleep mode
  sleep_cpu();
}

void wakeUpAgain()
{
  //stop sleep mode
  sleep_disable();
  //clear the interrupt
  detachInterrupt(interruptPin);
}