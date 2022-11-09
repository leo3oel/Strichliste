#include "direction.h"

Direction::Direction(unsigned short int nclk, unsigned short int ndt, unsigned short int nsw)
{
    clk = nclk;
    dt = ndt;
    sw = nsw;
    pinMode(clk, INPUT);
    pinMode(dt, INPUT);
    pinMode(sw, INPUT);
    digitalWrite(sw, HIGH);
    digitalWrite(dt, HIGH);
    digitalWrite(clk, HIGH);
}


bool Direction::turnedClockwise()
{
    prevStateclkCW = stateclkCW;
    stateclkCW = digitalRead(clk);
    statedtCW = digitalRead(dt);
    if(prevStateclkCW == 1 && stateclkCW == 0 && statedtCW == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

bool Direction::turnedCounterClockwise()
{
    prevStatedtCCW = statedtCCW;
    stateclkCCW = digitalRead(clk);
    statedtCCW = digitalRead(dt);
    if(prevStatedtCCW == 1 && statedtCCW == 0 && stateclkCCW == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

bool Direction::buttonState()
{
    bool buttonState = digitalRead(sw);
    return buttonState;
}