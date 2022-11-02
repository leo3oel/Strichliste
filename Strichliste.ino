#include <LiquidCrystal.h>
#include <avr/sleep.h>//going to sleep
#include <EEPROM.h>//for saving data
#include <pin.h> //Custom Pin Library https://github.com/leo3oel/pin
#define ARRAYSIZE 60 //set max number of people
#define interruptPin 2

//Variablen und Konstanten

const unsigned short int lcdRS = 12, lcdEN = 11, lcdD4 = 7, lcdD5 = 8, lcdD6 = 9, lcdD7 = 10; 
LiquidCrystal lcd(lcdRS, lcdEN, lcdD4, lcdD5, lcdD6, lcdD7);

//Pins
DigitalPin StrichePlus(3,INPUT), StricheMinus(4, INPUT), StricheReset(6, INPUT);
DigitalPin Maintenance(5, INPUT), NameDown(15, INPUT), NameUp(1, INPUT);
DigitalPin LCDled(14, OUTPUT);

unsigned long int inactivity=0; //sleeptimer
short int disteeprom=16;//distance between persons in EEPROM

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
  if(inactivity>2000)//Nach ca. 20s
  {
    inactivity = 0;
    PosInList=-1;
    sendToSleep();
    lcd.setCursor(0,1);
    lcd.print(F("Bitte auswaehlen"));
  }

  //Hoch/runter schalten
  if(NameDown.posEDGE())//PosInList durchwählen positiv
  {
    inactivity = 0;
    PosInList++;
  }

  if(NameUp.negEDGE())//PosInList durchwählen negativ
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
    int i=0;

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
    lcd.print(F("zum loeschen"));
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
  delay(50);//20ms delay
}