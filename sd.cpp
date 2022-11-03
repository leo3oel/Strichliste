#ifndef _SPI_H
#define _SPI_H
#include <SPI.h>
#endif

#ifndef _SD_H
#define _SD_H
#include <SD.h>
#endif

void writesd()
{
    
}

unsigned int countfiles()
{
    unsigned int counter = 0;
    char[11] backup = "BACKUP0.txt";
    while(SD.exists(backup))
    {
        counter++;
        backup[6] = counter;
    }
} 