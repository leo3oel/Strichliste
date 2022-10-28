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
 *  - 
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
int switchState = 0;
int prevSwitchState = 0;

//Namen hochzählen
int nschalterstat = 0;
int prevnschalterstat = 0;

//Listenposition
int posinlist = -1; //Zählt personen durch; -1 ist der bitte auswählen bildschirm
int posinlistprev=0;

//Striche zurücksetzen
int resetS=0;
int resetSprev=0;

//Striche runterzählen
int switchStateBack = 0;
int prevswitchStateBack = 0;

//Namen runterzählen
int nschalterstatBack = 0;
int prevnschalterstatBack =0;

//Wartungsmodus
int maintenanceState = 0;
int prevmaintenanceState = 0;

//Arrays für Striche/Namen
int striche[ARRAYSIZE]; //= {6,0,12,0,1,0,0};//Striche
char stnamen[ARRAYSIZE][12];// = {"Leonhard S", "Sophie M", "Rainer S", "Georg S", "Simon S"};//Array mit allen Namen, länge auf 12 begrenzt
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
void maintenance();//Wartungsmodus, input neuer listeneinträge per Serial


void setup() //Setup
{

  //!!!!!!!!!!!!!
  //Nur um neue Mitglieder anzulegen
  //Später bessere Lösung finden:
  //Serial.begin(9600);
  //EEPROM schreiben
  //writeEEPROM();
 
  //EEPROM lesen 
  for(int i = 0; i<ARRAYSIZE; i++)
  {
    int ieeprom = i*disteeprom;
    striche[i] = EEPROM.read(ieeprom); //Striche aus EEPROM lesen
    for(int j = 0; j<12; j++)
    {
      stnamen[i][j] = EEPROM.read(ieeprom+j+4); //Einzelne Character aus EEPROM in die Strings einlesen
    }
  }

  //countarraylength
  usedpers=0;
  for(int i=0; i<ARRAYSIZE; i++)
    if(stnamen[i][0]!=0)
      usedpers++;

  //Serial.print(usedpers);
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
  if(inactivity>500000)
  {
    inactivity = 0;
    posinlist=-1;
    sort();
    //sendToSleep();
    lcd.setCursor(0,1);
    lcd.print("Bitte auswaehlen");
  }
{  //Pins lesen
  nschalterstat = digitalRead(nschalter);
  switchState  = digitalRead(switchPin);
  resetS = digitalRead(resetP);
  switchStateBack = digitalRead(switchBackPin);
  nschalterstatBack = digitalRead(nschalterBackPin);
  maintenanceState = digitalRead(maintenancePin);
}
  
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

  {//Striche hoch/runterzählen/zurücksetzen
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
}
  
if(prevmaintenanceState != maintenanceState)
  maintenance();

  //Vorgängerzustände setzen
  prevnschalterstat = nschalterstat;
  posinlistprev = posinlist;
  prevSwitchState = switchState;
  resetSprev = resetS;
  prevswitchStateBack = switchStateBack;
  prevnschalterstatBack = nschalterstatBack;
  prevmaintenanceState = maintenanceState;
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
    //Serial.println("Schreiben");
    for(int i=0; i<ARRAYSIZE; i++)
    {
      int ieeprom = i*disteeprom;
      EEPROM.update(ieeprom, striche[i]);//Striche schreiben
      for(int j=0; j<12; j++)
      {
          EEPROM.update(((ieeprom+4)+j),stnamen[i][j]);
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

void maintenance()
{
  //Display löschen
  long int des=0;
  long int del = 0;
  int count = 0;
  lcd.clear();
  lcd.print("Wartungsmodus");
  lcd.setCursor(0,1);
  lcd.print("Serial 9600");

  Serial.begin(9600);
  Serial.println("----------------------------");
  Serial.println("!Achtung lange Wartezeiten!");
  Serial.println("Löschen oder Hinzufügen?");
  Serial.println("1: Löschen");
  Serial.println("2: Hinzufügen");
  while(des==0)
  {
    des = Serial.parseInt(SKIP_ALL, '\n');
    count++;
    delay(2000);
    if(count>10)
      break;
  }
  if (des == 1)
  {
    for(int i=0; i < usedpers; i++)
    {
      Serial.print(i+1);
      Serial.print(": Namen:");
      Serial.println(stnamen[i]);
    }
    Serial.println("Nummer zum Löschen eingeben, zum Abbrechen Zahl größer oder kleiner eingeben");
    while (del == 0)
    {
      del = Serial.parseInt(SKIP_ALL, '\n');
      count++;
      delay(2000);
      if(count>10)
        break;
    }
    if (del>=1 && del <= usedpers)
    {
      for (int i = del-1; i<usedpers; i++)//Array zusammenstauchen
      {
        striche[i]=striche[i+1];//Striche jeweils eins nach vorne schreiben
        for (int j = 0; j < 12; j++)
          stnamen[i][j] = stnamen[i+1][j];//Namen jeweils eins nach vorne schreiben
      }
      usedpers--;
      Serial.println("Person gelöscht");
    }
    else
    {
      Serial.println("Falsche Nummer, exit");
      exit;
    }
  }
  else if (des == 2)
  {//Neue person hinten anlegen
    if(usedpers == ARRAYSIZE)
    {
      Serial.println("Array voll");
    }
    else
    {
      Serial.println("Achtung max 12 Zeichen");;
      Serial.println("Namen eingeben:");
      String in;
      while (in==0)
      {
        in = Serial.readString();
        count++;
        delay(2000);
        if(count>10)
          break;
      }
      for (int i=0;i<12;i++)
        if (in[i] != '\n')    
          stnamen[usedpers][i] = in[i];
      usedpers++;
      striche[usedpers-1] =0;
      Serial.println("Person hinzugefügt");
    }
  }
  else
  {
    Serial.print("Falsche Nummer, exit");
    exit;
  }
  
  sort();
  posinlist = -1;
  lcd.clear();
  lcd.print("Name:");
  lcd.setCursor(8,0);
  lcd.print("Striche:");
}