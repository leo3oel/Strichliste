#include "ownsd.h"

void writesd()
{
    SD.begin(10);
    MyFile = SD.open("Backup.txt", FILE_WRITE);//Datei öffnen

    MyFile.println(F("Name/Striche Darstellung:"));
    for(short int i = 0; i<usedpers; i++)
    {
        MyFile.print(F("Name: "));
        MyFile.print(namesarray[i]);
        MyFile.print(F(" Striche: "));
        MyFile.println(strichearray[i]);
    }
    linebreak();
    MyFile.println(F("Array-Darstellung"));
    MyFile.print(F("strichearray[ARRAYSIZE] = ["));
    MyFile.print(strichearray[0]);
    for(short int i = 1; i<ARRAYSIZE; i++)
    {
        MyFile.print(F(", "));
        MyFile.print(strichearray[i]);
    }
    MyFile.println(F("];"));

    MyFile.print(F("namesarray[ARRAYSIZE][12] = ["));
    MyFile.print(namesarray[0]);
    MyFile.print(F("\""));
    for(short int i = 1; i<ARRAYSIZE; i++)
    {
        MyFile.print(F(", "));
        MyFile.print(F("\""));
        MyFile.print(namesarray[i]);
        MyFile.print(F("\""));
    }
    MyFile.println(F("];"));
    linebreak();
    linebreak();
    MyFile.println(F("END"));
    MyFile.close();
}

void linebreak()
{
    for(short int i=0;i<30;i++)
    {
        MyFile.print(F("-"));
    }
    MyFile.println(F("-"));
}