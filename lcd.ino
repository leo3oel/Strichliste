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

void schreiben (int a)//funktion zum schreiben des lcd displays
{
  if(a!=-1)
  {
    lcd.clear();
    lcd.print("Name:");
    lcd.setCursor(8,0);
    lcd.print("Striche:");
    lcd.setCursor(0,1);
    lcd.print(namesarray[a]);
    lcd.setCursor(14,1); 
    lcd.print(strichearray[a]);
  }
  else if (a==-1)
  {
    lcd.clear();
    lcd.print("Name:");
    lcd.setCursor(8,0);
    lcd.print("Striche:");
    lcd.setCursor(0,1);
    lcd.print(F("Bitte auswaehlen"));
  }
}