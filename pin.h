class Pin
{
    protected:
        unsigned short int pinnumber;

    public:
        
};

class Digitalpin : Pin
{
    private:
        bool currentstate;
        bool previousstate;

    public:
        bool isHIGH();
        bool isLOW();
        bool EDGE();
        bool posEDGE();
        bool negEDGE();
        Digitalpin (unsigned short int);
};