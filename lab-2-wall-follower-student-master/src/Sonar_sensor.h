#ifndef SONAR_SENSOR
#define SONAR_SENSOR

#include <Romi32U4.h>

class SonarSensor{
    private:
        const int pin_TRIG = 0;
        const int pin_ECHO = 1;
        
        bool pinged = false;
        

        const float microsToDistance = 0.01715f;
        const unsigned long waitPeriod = 1000,
                            timeoutPeriod = 100000;

        unsigned long sonarOut = 0,
                    lastSonarTime = 0,
                    sonarWaitStart = 0;

        float distance;

    public:
        void Init(void); 
        float ReadData(void); 
        void PrintData(void);

        void update(void);
        void echo();
        void ping();

        static void Update(void);
};

#endif