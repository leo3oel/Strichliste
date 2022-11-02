#include <LiquidCrystal.h>
#include <avr/sleep.h>//going to sleep
#include <EEPROM.h>//for saving data
#include "pindefinition.cpp"
#define ARRAYSIZE 60 //set max number of people
#define interruptPin 2

//Variablen und Konstanten


LiquidCrystal lcd(PinLCDrs, PinLCDen, PinLCDd4, PinLCDd5, PinLCDd6, PinLCDd7);

//Pins
DigitalPin Maintenance(5, INPUT), StrichePlus(3,INPUT);

unsigned long int inactivity=0; //sleeptimer
short int disteeprom=16;//distance between persons in EEPROM

//Striche ++
//bool StateStrichePlus = 0; //Current state of + Button
//bool prevStateStrichePlus = 0; //Previous state of + Button

//Striche --
bool StateStricheMinus = 0; //Current state of - Button
bool prevStateStricheMinus = 0; //Previous state of - Button

//Striche = 0
bool StateStricheReset=0; //Current state of reset Button
bool prevStateStricheReset=0; //Previous state of - Button

//Names down (A->Z)
bool StateNamesDown = 0; //Current state of Names Down Button
bool prevStateNamesDown = 0; //Previous state of Names Down Button

//Names up (Z->A)
bool StateNamesUp = 0; //Current state of Names Up Button
bool prevStateNamesUp =0; //Previous state of Names Up Buton

//Maintenance Mode
//bool StateMaintenance = 0;
//bool prevStateMaintenance = 0;

//Current Position in List
short int PosInList = -1; //Current position in List, -1 => display "make a selection"
short int prevPosInList = 0; //Previous position in List

//Arrays for Striche/Names
unsigned short int strichearray[ARRAYSIZE]; //= {6,0,12,0,1,0,0};//Striche
char namesarray[ARRAYSIZE][12]; //Array mit all Names, max 12 Characters per Name
short int usedpers=0; //Currently used Places in Array


void setup() //Setup
{
	//Serial.begin(9600);
  //EEPROM lesen
  readEEPROM();

  //countarraylength
  countarraylength();
  // Serial.print(usedpers);
  lcd.begin(16,2);
  //pinMode(PinStrichePlus,INPUT);
  pinMode(PinNameGoDown,INPUT);
  pinMode(PinStricheReset,INPUT);
  pinMode(PinStricheMinus, INPUT);
  pinMode(PinNameGoUp, INPUT);
  //pinMode(PinMaintenance, INPUT);
	pinMode(PinLCDled, OUTPUT);
  pinMode(interruptPin, INPUT); //Pin for wakeup
	digitalWrite(PinLCDled, HIGH);
  lcd.print("Name:");
  lcd.setCursor(8,0);
  lcd.print("Striche:");
  lcd.setCursor(0,1);
  lcd.print("Bitte auswaehlen");
  sort();
}


void loop() {
    //sleep inactivity abfrage nach gefühl
    if(inactivity>2000)//Nach ca. 20s
    {
      inactivity = 0;
      PosInList=-1;
      sendToSleep();
      lcd.setCursor(0,1);
      lcd.print("Bitte auswaehlen");
    }

    //Pins lesen
    StateNamesDown = digitalRead(PinNameGoDown);
    //StateStrichePlus  = digitalRead(PinStrichePlus);
    StateStricheReset = digitalRead(PinStricheReset);
    StateStricheMinus = digitalRead(PinStricheMinus);
    StateNamesUp = digitalRead(PinNameGoUp);
    //StateMaintenance = digitalRead(PinMainenance);
    //Hoch/runter schalten
    if(!prevStateNamesDown && StateNamesDown)//PosInList durchwählen positiv
    {
      inactivity = 0;
      PosInList++;
    }
    
    if(!prevStateNamesUp && StateNamesUp)//PosInList durchwählen negativ
    {
      inactivity = 0;
      PosInList--;
    }
    if(PosInList == -1)//Start
    {
      lcd.setCursor(0,1);
      lcd.print("Bitte auswaehlen");
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

    if(!prevStateStricheMinus && StateStricheMinus) //Zugehörige Striche hochzählen
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

    if(!prevStateStricheReset && StateStricheReset && PosInList>=0)//Striche zurücksetzen
    {
      inactivity = 0;
      
      prevStateStricheReset = StateStricheReset;
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("ACHTUNG!");
      lcd.setCursor(0,1);
      lcd.print(strichearray[PosInList]);
      lcd.setCursor(4,1);
      lcd.print("EUR bezahlt?");
      delay(2000);//2sec delay
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("zum loeschen");
      lcd.setCursor(0,1);
      lcd.print("gedrueckt halten");
      delay(2000);//3 sec delay, danach erneute abfrage ob noch gedrückt
      if(digitalRead(PinStricheReset))
      {
        strichearray[PosInList]=0;
      }
      schreiben(PosInList);
      EEPROM.update((PosInList)*disteeprom,strichearray[PosInList]);
    }
    
    
    if(Maintenance.posEDGE())
    {
      maintenance();
    }
    //Vorgängerzustände setzen
    prevStateNamesDown = StateNamesDown;
    prevPosInList = PosInList;
    //prevStateStrichePlus = StateStrichePlus;
    prevStateStricheReset = StateStricheReset;
    prevStateStricheMinus = StateStricheMinus;
    prevStateNamesUp = StateNamesUp;
    //prevStateMaintenance = StateMaintenance;
    inactivity++;
    delay(20);//20ms delay
}