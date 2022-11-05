/* 
    Funktionen zum Schreiben des Displays
*/

void zeileloeschen (int a)//Löscht Display
{
  for(int i=0; i<=16; i++)
  {
    lcd.setCursor(i,a);
    lcd.print(" ");
  }
}

void schreiben (short newa, unsigned short poseeTOposlcd[])//funktion zum schreiben des lcd displays
{
  if(newa!=-1)
  {
    short a = poseeTOposlcd[newa];
    lcd.clear();
    lcd.print(F("Name:"));
    lcd.setCursor(8,0);
    lcd.print(F("Striche:"));
    lcd.setCursor(0,1);
    lcd.print(namesarray[a]);
    lcd.setCursor(14,1); 
    lcd.print(strichearray[a]);
  }
  else if (newa==-1)
  {
    lcd.clear();
    lcd.print(F("Name:"));
    lcd.setCursor(8,0);
    lcd.print(F("Striche:"));
    lcd.setCursor(0,1);
    lcd.print(F("Bitte auswaehlen"));
  }
}