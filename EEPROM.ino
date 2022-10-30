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
    for(int i=0; i<ARRAYSIZE; i++)
    {
      int ieeprom = i*disteeprom;
      EEPROM.update(ieeprom, striche[i]);//Striche schreiben
      for(int j=0; j<12; j++)
      {
          EEPROM.update(((ieeprom+4)+j),stnamen[i][j]);
      }
    }
}

void sort() //Sortiert und schreibt auf EEPROM
{
  /*
   * Sortiert Striche und Namen parallel im double Sort.
   * Die sortierten Arrays werden auf den EEPROM geschriben
   */

  char tempnamen[12];
  int tempstriche=0;
  for(int i=0; i<usedpers; i++)
  {
    for(int j=0;j<usedpers-1; j++)
    {
      if ((stnamen[j][0]==stnamen[j+1][0] && stnamen[j][1]>stnamen[j+1][1])||(stnamen[j][0]>stnamen[j+1][0]))
      {
        for(int c=0; c<12;c++)
        {
          //Namen tauschen
          tempnamen[c] = stnamen[j][c];
          stnamen[j][c] = stnamen[j+1][c];
          stnamen[j+1][c] = tempnamen[c];
        }
        //Striche tauschen
        tempstriche = striche[j];
        striche[j] = striche[j+1];
        striche[j+1] = tempstriche;
      }
    }
  }
  writeEEPROM();
}

void countarraylength()
{
  usedpers=0;
  for(int i=0; i<ARRAYSIZE; i++)
    if(stnamen[i][0]!=0)
      usedpers++;
}

void readEEPROM()
{
  for(int i = 0; i<ARRAYSIZE; i++)
  {
    int ieeprom = i*disteeprom;
    striche[i] = EEPROM.read(ieeprom); //Striche aus EEPROM lesen
    for(int j = 0; j<12; j++)
    {
      stnamen[i][j] = EEPROM.read(ieeprom+j+4); //Einzelne Character aus EEPROM in die Strings einlesen
    }
  }
}