void resetHands()
{
  // Hours
  while(!digitalRead(M_ORE))
    stepperH.step(2);
  while(digitalRead(M_ORE))
    stepperH.step(2);

  // Minutes
  while(!digitalRead(M_MIN))
    stepperM.step(2);
  while(digitalRead(M_MIN))
    stepperM.step(2);
}

void setTime()
{
  // Hours
  int now = ntp.hours();
  float tmp = 0.0;
  if(now >12)
  {
    tmp = now * 170.66667;
    stepperH.step(tmp);
  } else {
    tmp = (now - 12.0) * 170.66667;
    stepperH.step(tmp);
  }

  // Minutes
  stepperM.step((ntp.minutes() * 34.4));
}
