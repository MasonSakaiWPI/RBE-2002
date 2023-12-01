#include <Romi32U4.h>
#include "chassis.h"

float RomiChassis::SpeedLeft(void)
{
    // !!! ATTENTION !!!
    // Assignment 1

    // Vt = 2r(pi)/N * (n/T)
    // units should be mm/s
    
    return C_wheel * (count_left - prev_count_left) / (N_wheel * interval * 1E-3); //[mm/s]
}

float RomiChassis::SpeedRight(void)
{
    // !!! ATTENTION !!!
    // Assignment 1

    // Vt = 2r(pi)/N * (n/T)
    // units should be mm/s
    
    return C_wheel * (count_right - prev_count_right) / (N_wheel * interval * 1E-3); //[mm/s]
}

void RomiChassis::UpdateEffortDriveWheels(int left, int right)
{ 
    motors.setEfforts(left,right);
}

void RomiChassis::UpdateEffortDriveWheelsPI(int target_speed_left, int target_speed_right)
{
  // !!! ATTENTION !!!
  // Assignment 2
  // define the PI controller
  // use the chassis.h accumulator error and define an error deltaLeft and deltaRight


  float v_left = SpeedLeft(); //get current speed
  float v_right = SpeedRight();

  float d_left = target_speed_left - v_left; //establish error between target speed and current speed
  float d_right = target_speed_right - v_right;
  
  float u_left = Kp * d_left + Ki * E_left; //establish PI equation
  float u_right = Kp * d_right + Ki * E_right;

  E_left += d_left; //update accumulated error
  E_right += d_right;

  chassis.SerialPlotter(millis(), v_left, u_left, d_left);
  
  motors.setEfforts(u_left,u_right); //set speed
}

void RomiChassis::SerialPlotter(float a, float b, float c, float d)
{
    // !!! ATTENTION !!!
    // USE this function for assignment 3!
    //Speed over time graph, effort over time, error over time, for each wheel
    Serial.print(a);
    Serial.print('\t');
    Serial.print(b);
    Serial.print('\t');
    Serial.print(c);
    Serial.print('\t');
    Serial.print(d);
    Serial.println();
}

void RomiChassis::MotorControl(void)
{
  uint32_t now = millis();
  if(now - last_update >= interval)
  {    
    prev_count_left = count_left;
    prev_count_right = count_right;
    count_left = encoders.getCountsLeft();
    count_right = encoders.getCountsRight();
    previous_time = millis();
    UpdateEffortDriveWheelsPI(target_left, target_right);
    last_update = now;
  }
}

void RomiChassis::StartDriving(float left, float right, uint32_t duration)
{
  target_left = left; target_right = right;
  start_time = millis();
  last_update = start_time;
  end_time = start_time + duration; //fails at rollover
  E_left = 0;
  E_right = 0;
}

bool RomiChassis::CheckDriveComplete(void)
{
  return millis() >= end_time;
}

void RomiChassis::Stop(void)
{
  target_left = target_right = 0;
  motors.setEfforts(0, 0);
}