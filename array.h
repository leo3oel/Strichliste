#include "Arduino.h"
#include <EEPROM.h>
#include <string.h>
//externe Variablen
#ifndef ARRAYSIZES
#define ARRAYSIZE 60
#define STRINGLENGTH 12
#endif


extern const short disteeprom;


//Funktionen
void writeEEPROM(unsigned short striche[]);
void sort(char names[][STRINGLENGTH], unsigned short poseeTOposlcd[]);
unsigned short countarraylength(char names[][STRINGLENGTH]);//Zählt benutzte Array felder
void readEEPROM(unsigned short striche[]);