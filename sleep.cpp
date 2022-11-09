#include "sleep.h"

/*
  Ermöglicht das Schlafenlegen und Wecken des Controllers
  https://thekurks.net/blog/2018/1/24/guide-to-arduino-sleep-mode
*/

void sendToSleep(char names[][STRINGLENGTH], unsigned short striche[])
{
    // enable sleeping - note this primes sleep, not starts it!
    sleep_enable();
    // attach the interrupt, specify the pin, the method to call on interrupt,
    // and the interrupt conditions, in this case when the pin is pulled low.
    pinMode(2, INPUT);
    digitalWrite(2, true);
    attachInterrupt(digitalPinToInterrupt(2), wakeUpAgain, LOW); // Interrupt 0 is connected to digital pin 2
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);                         // Full Sleep
    lcd.noDisplay();
    readEEPROM(striche); // Werte aktualisieren
    writesd(names, striche);
    LCDled.setOut(LOW);
    delay(5000);
    // actually activate sleep mode
    sleep_cpu();
    // Serial.println("just woke up");
    LCDled.setOut(HIGH);
    lcd.display();
}

void wakeUpAgain()
{
    // Serial.println("Interrupt fired");
    // stop sleep mode
    sleep_disable();
    // clear the interrupt
    detachInterrupt(0);
}
