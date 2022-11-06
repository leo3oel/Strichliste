#include "ownsd.h"

bool readSD(char names[][STRINGLENGTH])
{    
    SD.begin(10);
    File ReadFile; // Object File
    if (SD.exists("names.txt"))//check if file exists
    {
        ReadFile = SD.open("names.txt");//open file
        for (int i = 0; i < ARRAYSIZE; i++)
        {
            char temp = 0;
            short j = 0;
            while ((int)temp != 59 && ReadFile.available())
            {
                // Runs until ";" is read or File at end
                temp = ReadFile.read();
                if ((temp > 64 && temp < 123) || (temp > 47 && temp < 58) || temp == 32)
                {
                    // allowed chars: numbers, letters, spaces
                    names[i][j] = (char)temp;
                    j++;
                }
                if (j > 12)
                    break; // breaks if string is too long
            }
        }
        ReadFile.close();
        return 1;
    }
    else
        return 0;
}

void writesd(char names[][STRINGLENGTH], unsigned short striche[]) //übergabe array per Referenz
{
    File MyFile;
    SD.begin(10);
    MyFile = SD.open("Backup.txt", FILE_WRITE); // Datei öffnen
    //print normal style
    MyFile.println(F("Name/Striche Darstellung:"));
    for (short int i = 0; i < usedpers; i++)
    {
        MyFile.print(F("Name: "));
        MyFile.print(names[i]);
        MyFile.print(F(" Striche: "));
        MyFile.println(striche[i]);
    }
    linebreak(MyFile);
    MyFile.println(F("Array-Darstellung"));
    MyFile.print(F("strichearray[ARRAYSIZE] = ["));
    MyFile.print(striche[0]);
    for (short int i = 1; i < ARRAYSIZE; i++)
    {
        MyFile.print(F(", "));
        MyFile.print(striche[i]);
    }
    MyFile.println(F("];"));
    //print array style
    MyFile.print(F("namesarray[ARRAYSIZE][12] = ["));
    MyFile.print(names[0]);
    MyFile.print(F("\""));
    for (short int i = 1; i < ARRAYSIZE; i++)
    {
        MyFile.print(F(", "));
        MyFile.print(F("\""));
        MyFile.print(names[i]);
        MyFile.print(F("\""));
    }
    MyFile.println(F("];"));
    linebreak(MyFile);
    linebreak(MyFile);
    MyFile.println(F("END"));
    MyFile.close();
}

void linebreak(File MyFile)
{
    //horizontal bar
    for (short int i = 0; i < 30; i++)
    {
        MyFile.print(F("-"));
    }
    MyFile.println(F("-"));
}