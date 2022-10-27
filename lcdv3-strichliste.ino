#include <LiquidCrystal.h>
#include <avr/sleep.h>
//#include "functions.h"
#include<EEPROM.h>//Für dauerspeicher
//#include <RocketScream_LowPowerAVRZero.h>
#define ARRAYSIZE 50 //Anzahl der Personen festlegen
#define interruptPin 2
LiquidCrystal lcd(12, 11, 5, 4, 3, 9);

/*Gedanken: 
 *  k - array mit posinlist und array mit strichen gemeinsam auf EEPROM speichern
 *  - beide arrays gemeinsam nach größe sortieren
 *    - sortierung muss leere arrayzellen nach hinten stellen
 *  - andere lösung für bildschirmtime out suchen (unabhängig von name machen)
 *    - zB timeout variable einführen:
 *      - um das timeout zu verlassen knopf betätigen
 *      - danach name auf 0 setzen
 *      - timeout variable zurück setzen
 *    - alternativ über sleepfunktion
 *    - zum schluss umsetzen
 *  - FRAGE: speichern von strings auf EEPROM möglich?
 *    - character einzeln speichern
 *    - strings werden auf max 12 character begrenzt
 *    - int benötigt 4 bytes char 1 byte
 *      -> pro person 4+12*1=16bytes
 *    - 1024 bytes speicher -> 64 personen max möglich
 *  - möglichkeit zum posinlist neu anlegen einführen
 *    - zB Wartungsmodus:
 *      - Output auf SD Karte
 *      - In Reihenfolge wie aktuell sortiert
 *      - Format so wählen, dass leere Arrayzellen mit ausgegeben werden
 *      - posinlist in die leeren Arrayzellen schreiben
 *      - posinlist und Striche von SD Karte einlesen
 *      - direkt auf EEPROM speichern
 * 
 *    
 * 
 */


//Variablen und Konstanten
const int switchPin = 6; // Schalter
const int nschalter = 7;
const int resetP = 8;
//const int wakeUpPin = 6;

int switchState = 0;
int prevSwitchState = 0;

int nschalterstat = 0;
int prevnschalterstat = 0;

int posinlist = -1; //Zählt personen durch
int posinlistprev=0;
short int disteeprom=16;//abstand einzelner Personen im EEPROM

int resetS=0;
int resetSprev=0;

int striche[ARRAYSIZE] = {6,0,12,0,1,0,0};//Striche
char stnamen[ARRAYSIZE][12] = {"Leonhard S", "Sophie M", "Rainer S", "Georg S", "Simon S"};//Array mit allen Namen, länge auf 12 begrenzt
int usedpers=0;

unsigned long int inactivity=0; //sleeptimer


void schreiben (int);
void zeileloeschen (int a)
{
  for(int i=0; i<=16; i++)
  {
    lcd.setCursor(i,a);
    lcd.print(" ");
  }
}

void sort();
void writeEEPROM();
void sendToSleep();
void wakeUpAgain();

void setup() {

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
  //sleep
  if(inactivity>10000)
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
  
  if(posinlist == -1)
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
    delay(5000);
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