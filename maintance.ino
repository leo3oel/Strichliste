/* 
    Wartungsmodus ermöglicht Anlegen neuer Personen, bzw. das löschen von Personen über die Serial Schnittstelle
    Nach dem Anlegen/Löschen wird die Strichliste neu sortiert.
*/

void maintenance()
{
  //Display löschen
  long int des=0;
  long int del = 0;
  int count = 0;
  lcd.clear();
  lcd.print("Wartungsmodus");
  lcd.setCursor(0,1);
  lcd.print("Serial 9600");

  Serial.begin(9600);
  Serial.println("----------------------------");
  Serial.println("!Achtung lange Wartezeiten!");
  Serial.println("Löschen oder Hinzufügen?");
  Serial.println("1: Löschen");
  Serial.println("2: Hinzufügen");
  while(des==0)
  {
    des = Serial.parseInt(SKIP_ALL, '\n');
    count++;
    delay(2000);
    if(count>10)
      break;
  }
  if (des == 1)
  {
    for(int i=0; i < usedpers; i++)
    {
      Serial.print(i+1);
      Serial.print(": Namen: ");
      Serial.println(namesarray[i]);
    }
    Serial.println("Nummer zum Löschen eingeben, zum Abbrechen Zahl größer oder kleiner eingeben");
    while (del == 0)
    {
      del = Serial.parseInt(SKIP_ALL, '\n');
      count++;
      delay(2000);
      if(count>10)
        break;
    }
    if (del>=1 && del <= usedpers)
    {
      for (int i = del-1; i<usedpers; i++)//Array zusammenstauchen
      {
        strichearray[i]=strichearray[i+1];//Striche jeweils eins nach vorne schreiben
        for (int j = 0; j < 12; j++)
          namesarray[i][j] = namesarray[i+1][j];//Namen jeweils eins nach vorne schreiben
      }
      usedpers--;
      Serial.println("Person gelöscht");
    }
    else
    {
      Serial.println("Falsche Nummer, exit");
      exit;
    }
  }
  else if (des == 2)
  {//Neue person hinten anlegen
    if(usedpers == ARRAYSIZE)
    {
      Serial.println("Array voll");
    }
    else
    {
      Serial.println("Achtung max 12 Zeichen");;
      Serial.println("Namen eingeben:");
      String in;
      while (in==0)
      {
        in = Serial.readString();
        count++;
        delay(2000);
        if(count>10)
          break;
      }
      for (int i=0;i<12;i++)
        if (in[i] != '\n')    
          namesarray[usedpers][i] = in[i];
      usedpers++;
      strichearray[usedpers-1] =0;
      Serial.println("Person hinzugefügt");
    }
  }
  else
  {
    Serial.print("Falsche Nummer, exit");
    exit;
  }
  
  sort();
  PosInList = -1;
  lcd.clear();
  lcd.print("Name:");
  lcd.setCursor(8,0);
  lcd.print("Striche:");
}