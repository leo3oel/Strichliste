#include "array.h"

void writeEEPROM(unsigned short striche[])
{
	for(short  i=0; i<ARRAYSIZE; i++)
	{
		short ieeprom = i*disteeprom;
		EEPROM.update(ieeprom, striche[i]);//Striche schreiben
	}
}

void sort(char names[][STRINGLENGTH], unsigned short poseeTOposlcd[]) //Schreibt das poseeTOposlcd array
{
  /*
   * Sortiert Striche und Namen parallel im double Sort.
   * Die sortierten Arrays werden auf den EEPROM geschriben
   */
	short minnumber;
	char* newbottom = "AAAAAAAAAAA";
	for (short i = 0; i < ARRAYSIZE; i++)
	{
		char *kleinsterstring = "zzzzzzzzzzz";
		for (short j = 0; j < ARRAYSIZE; j++)
		{
			// kleiner als zzz aber größer als vorheriger string
			if ((strcmp(names[j], kleinsterstring) < 0) && (strcmp(names[j], newbottom) > 0))
			{
				kleinsterstring = names[j];
				minnumber = j;
			}
		}
		if (minnumber != poseeTOposlcd[i - 1] || i == 0)
		{
			newbottom = names[minnumber];
			poseeTOposlcd[i] = minnumber;
		}
		else
		{
			poseeTOposlcd[i] = -1;
			minnumber = -1;
		}
	}
}

unsigned short countarraylength(char names[][12])
{
  unsigned short usedpers=0;
  for(short i=0; i<ARRAYSIZE; i++)
    if(names[i][0]!=0)
      usedpers++;
  return usedpers;
}

void readEEPROM(unsigned short striche[])
{
  for(short i = 0; i<ARRAYSIZE; i++)
  {
    short ieeprom = i*disteeprom;
    striche[i] = EEPROM.read(ieeprom); //Striche aus EEPROM lesen
  }
}