/* 
    Funktionen für EEPROM
    Sortierfunktion
    Schreibfunktion
*/

void writeEEPROM()
{
    /*
     * Schreibt die kompletten Arrays auf das EEPROM
     * Später noch das schreiben der SD Karte hier ergänzen
     */ 
    //Serial.println("Schreiben");
    for(short int i=0; i<ARRAYSIZE; i++)
    {
      int ieeprom = i*disteeprom;
      EEPROM.update(ieeprom, strichearray[i]);//Striche schreiben
      for(int j=0; j<12; j++)
      {
          EEPROM.update(((ieeprom+4)+j),namesarray[i][j]);
      }
    }
}

void sort() //Sortiert und schreibt auf EEPROM
{
  /*
   * Sortiert Striche und Namen parallel im double Sort.
   * Die sortierten Arrays werden auf den EEPROM geschriben
   */
  countarraylength();
  char tempnamen[12];
  short int tempstriche=0;
  for(short int i=0; i<usedpers; i++)
  {
    for(short int j=0;j<usedpers-1; j++)
    {
      if ((namesarray[j][0]==namesarray[j+1][0] && namesarray[j][1]>namesarray[j+1][1])||(namesarray[j][0]>namesarray[j+1][0]))
      {
        for(short int c=0; c<12;c++)
        {
          //Namen tauschen
          tempnamen[c] = namesarray[j][c];
          namesarray[j][c] = namesarray[j+1][c];
          namesarray[j+1][c] = tempnamen[c];
        }
        //Striche tauschen
        tempstriche = strichearray[j];
        strichearray[j] = strichearray[j+1];
        strichearray[j+1] = tempstriche;
      }
    }
  }
  writeEEPROM();
}

void countarraylength()
{
  usedpers=0;
  for(short int i=0; i<ARRAYSIZE; i++)
    if(namesarray[i][0]!=0)
      usedpers++;
}

void readEEPROM()
{
  for(short int i = 0; i<ARRAYSIZE; i++)
  {
    short int ieeprom = i*disteeprom;
    strichearray[i] = EEPROM.read(ieeprom); //Striche aus EEPROM lesen
    for(short int j = 0; j<12; j++)
    {
      namesarray[i][j] = EEPROM.read(ieeprom+j+4); //Einzelne Character aus EEPROM in die Strings einlesen
    }
  }
}