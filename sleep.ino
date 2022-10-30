/* 
    Ermöglicht das Schlafenlegen und Wecken des Controllers
    Noch nicht funktional
*/

void sendToSleep()
{
  sort();
  //enable sleeping - note this primes sleep, not starts it!
  sleep_enable();
  //attach the interrupt, specify the pin, the method to call on interrupt,
  //and the interrupt conditions, in this case when the pin is pulled low.
  attachInterrupt(interruptPin, wakeUpAgain, LOW);
  //actually activate sleep mode
  sleep_cpu();
}

void wakeUpAgain()
{
  //stop sleep mode
  sleep_disable();
  //clear the interrupt
  detachInterrupt(interruptPin);
}