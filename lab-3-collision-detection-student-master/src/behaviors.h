#ifndef BEHAVIORS
#define BEHAVIORS

#include <Romi32U4.h>

class Behaviors{
    private:
        int threshold = 175;
        int threshold_pick_up = 200;
        int data[3] = {0};
        enum ROBOT_STATE {IDLE, DRIVE, REVERSE, TURN};
        ROBOT_STATE robot_state = IDLE; //initial state: IDLE

        unsigned long driveStartMillis = 0;
        unsigned long driveStartTimeout = 500;
         
    public:
        void Init(void);
        void Stop(void);
        void Run(void);
        boolean DetectCollision(void);
        boolean DetectBeingPickedUp(void);
};

#endif