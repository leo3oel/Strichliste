#ifndef _SPI_H
#define _SPI_H
#include <SPI.h>
#endif

#ifndef _SD_H
#define _SD_H
#include <SD.h>
#endif

#ifndef ARRAYSIZES
#define ARRAYSIZE 60
#define STRINGLENGTH 12
#endif

#include "Arduino.h"

extern short int usedpers;
extern File MyFile;

void writesd(char names[][STRINGLENGTH], unsigned short striche[]);
void linebreak();