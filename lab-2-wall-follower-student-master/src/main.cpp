#include <Arduino.h>
#include <Romi32U4.h>
//sensors
#include "IR_sensor.h"
#include "Sonar_sensor.h"
#include "Encoders.h"
//behaviors
#include "Speed_controller.h"
#include "Wall_following_controller.h"

enum ROBOT_STATE {ROBOT_IDLE, ROBOT_DRIVING};
ROBOT_STATE robot_state = ROBOT_IDLE;

Romi32U4ButtonA buttonA; 
SpeedController PIcontroller;
WallFollowingController PDcontroller;

IRsensor irSensor;
SonarSensor sonar;

void setup() {
  PIcontroller.Init();
  PDcontroller.Init();
  irSensor.Init();
  sonar.Init();
}

int dist = 0;

void loop() {
  //sonar.update();
  //SonarSensor::Update();

  //sonar.PrintData();
  /*if(buttonA.getSingleDebouncedRelease()) {
    //irSensor.PrintData();
    dist += 5;
    Serial.println(dist);
    for(int i = 0; i < 20; i++) {
      //sonar.ping();
      irSensor.PrintData();
      delay(100);
    }
    Serial.println();
  }*/
  /*irSensor.PrintData();
  delay(100);*/
  switch(robot_state)
  {
    case ROBOT_IDLE:
      if(buttonA.getSingleDebouncedRelease()) robot_state = ROBOT_DRIVING;
      break;

    case ROBOT_DRIVING:
      //uncomment this line to check whether the speed controller is operational on your robot
      //PIcontroller.Process(50,50);

      //uncomment this line of code, once you are done with assignments 1 and 2 to demonstrate that your robot
      //is capable of following a wall autonomously.
      int speed = PDcontroller.Process(30); //distance in [cm]
      PIcontroller.Process(50+speed,50-speed); //speed in [[mm/s]]


      if(buttonA.getSingleDebouncedRelease()) 
      {
        PIcontroller.Stop();
        robot_state = ROBOT_IDLE;
      }
      break;
  }
}