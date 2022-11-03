#ifndef _SPI_H
#define _SPI_H
#include <SPI.h>
#endif

#ifndef _SD_H
#define _SD_H
#include <SD.h>
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE 50
#endif

#include "Arduino.h"

extern unsigned short int strichearray[ARRAYSIZE]; //= {6,0,12,0,1,0,0};//Striche
extern char namesarray[ARRAYSIZE][12]; //Array mit all Names, max 12 Characters per Name
extern short int usedpers;

void writesd();
unsigned short int countfiles();