#include "ownsd.h"

unsigned short int countfiles()
{
    unsigned int counter = 0, counterzw =0;
    char backup[13]  = "BACKUP000.txt";
    while(SD.exists(backup))
    {
        counter++;
        counterzw = counter;
        for(int i=8; i>=6; i--)
        {
            backup[i] = counterzw%10;
            counterzw/=10;
        }
    }
    return counter;
}

void writesd()
{
    SD.begin(10);
    File MyFile;


    unsigned short int counter = countfiles();
    char filename[13]  = "BACKUP000.txt";
    //Nummer anzeigen:
    for(int i=8; i>=6; i--)
    {
        filename[i] = counter%10;
        counter/=10;
    }
    MyFile = SD.open(filename, FILE_WRITE);//Datei öffnen

    MyFile.println(F("Name/Striche Darstellung:"));
    for(int i = 0; i<usedpers; i++)
    {
        MyFile.print(F("Name: "));
        MyFile.print(namesarray[i]);
        MyFile.print(F(" Striche: "));
        MyFile.print(strichearray[i]);
    }
    for(int i=0;i<30;i++)
    {
        MyFile.print(F("-"));
    }
    MyFile.println(F("-"));
    MyFile.println(F("Array-Darstellung"));
    MyFile.print(F("strichearray[ARRAYSIZE] = ["));
    MyFile.print(strichearray[0]);
    for(int i = 1; i<ARRAYSIZE; i++)
    {
        MyFile.print(F(", "));
        MyFile.print(strichearray[i]);
    }
    MyFile.println(F("];"));

    MyFile.print(F("\nnamesarray[ARRAYSIZE][12] = ["));
    MyFile.print(F("\""));
    MyFile.print(namesarray[0]);
    MyFile.print(F("\""));
    for(int i = 1; i<ARRAYSIZE; i++)
    {
        MyFile.print(F(", "));
        MyFile.print(F("\""));
        MyFile.print(namesarray[i]);
        MyFile.print(F("\""));
    }
    MyFile.println(F("];"));


}