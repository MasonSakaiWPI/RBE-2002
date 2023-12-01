#include <Romi32U4.h>
#include "Behaviors.h"
#include "Median_filter.h"
#include "IMU.h"
#include "Speed_controller.h"

//sensors
IMU_sensor LSM6;
Romi32U4ButtonA buttonA;

//median filter
MedianFilter med_x;
MedianFilter med_y;
MedianFilter med_z;

//motor-speed controller
SpeedController PIcontroller;

void Behaviors::Init(void)
{
    LSM6.Init();
    med_x.Init();
    med_y.Init();
    med_z.Init();
    PIcontroller.Init();
}

boolean Behaviors::DetectCollision(void)
{
    auto data_acc = LSM6.ReadAcceleration();
    data[0] = med_x.Filter(data_acc.X)*0.061f;
    data[1] = med_y.Filter(data_acc.Y)*0.061f;
    data[2] = med_z.Filter(data_acc.Z)*0.061f;
    if((abs(data[0]) > threshold) || (abs(data[1]) > threshold)) return 1;
    else return 0;
}

boolean Behaviors::DetectBeingPickedUp(void)
{
    //assignment 2
    auto data_acc = LSM6.ReadAcceleration();
    data[0] = med_x.Filter(data_acc.X)*0.061f;
    data[1] = med_y.Filter(data_acc.Y)*0.061f;
    data[2] = med_z.Filter(data_acc.Z)*0.061f;
    if((abs(data[2] - 1000) > threshold_pick_up)) return 1;
    else return 0;
}

void Behaviors::Stop(void)
{
    PIcontroller.Stop();
}

void Behaviors::Run(void)
{
    switch (robot_state)
    {
    case IDLE:
        if(buttonA.getSingleDebouncedRelease()){ //transition condition
            robot_state = DRIVE;
            driveStartMillis = millis();
        } 
        else { //transition condition
            robot_state = IDLE; 
            PIcontroller.Stop(); //action 
        }   
        break;
    case DRIVE: 
        PIcontroller.Run(250, 250);
        if(DetectBeingPickedUp() || buttonA.getSingleDebouncedRelease()){ //transition condition
            robot_state = IDLE;
            PIcontroller.Stop(); //action
        } else if(((millis() - driveStartMillis) > driveStartTimeout) && DetectCollision()) {
            robot_state = REVERSE;
            PIcontroller.Stop();
        }
        break;
    case REVERSE:
        PIcontroller.Reverse(100, 10);
        robot_state = TURN;
        break;
    case TURN:
        PIcontroller.Turn(90, 0);
        robot_state = DRIVE;
        driveStartMillis = millis();
        break;
        //assignment 3
    }
    //Serial.println(robot_state);
}