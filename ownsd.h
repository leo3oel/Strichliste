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

bool readSD(char names[][STRINGLENGTH]);//read names von sd
void writesd(char names[][STRINGLENGTH], unsigned short striche[]);//write name and striche
void linebreak(File MyFile);//print horizontal bar