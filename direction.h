#include "Arduino.h"

class Direction
{
    private:
        unsigned short int clk;
        unsigned short int dt;
        short int sw;
        bool stateclkCW = 0; //state of pin clk used in clockwise
        bool stateclkCCW = 0; //state of pin clk used in counter clockwise
        bool prevStateclkCW = 0; //previous state of pin clk used in clockwise
        bool statedtCW = 0; //state of pin dt used in clockwise
        bool statedtCCW = 0; //state of pin dt used in counterclockwise
        bool prevStatedtCCW = 0; //previous state of pin dt used in counterclockwise

    public:
        bool turnedClockwise(); //returns 1 after turning clockwise
        bool turnedCounterClockwise(); //returns 1 after turning counter clockwise
        bool buttonState(); // returns current button state
        Direction (unsigned short int, unsigned short int, unsigned short int);
};