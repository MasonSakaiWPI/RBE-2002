#include <Romi32U4.h>
#include "Encoders.h"
#include "Wall_following_controller.h"
#include "IR_sensor.h"
#include "Sonar_sensor.h"

IRsensor SharpIR;
SonarSensor HCSR04;
void WallFollowingController::Init(void)
{
    SharpIR.Init();
    HCSR04.Init();
}

float WallFollowingController::Process(float target_distance)
{
  //assignment 2: write a PD controller that outputs speed as a function of distance error
  float distance = SharpIR.ReadData();

  float e_distance = target_distance - distance;

  if(e_distance > 50) e_distance = 50;
  else if(e_distance < -50) e_distance = -50;

  float speed = Kp * e_distance + Kd * (e_distance - prev_e_distance);

  prev_e_distance = e_distance;

  return speed;
}