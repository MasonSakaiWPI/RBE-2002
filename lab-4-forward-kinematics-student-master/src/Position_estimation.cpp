#include  "Position_estimation.h"
#include "Encoders.h"

Encoder RomiEncoders;
float x = 0;
float y = 0;
float theta = 0;
unsigned long time_prev = millis();
unsigned long time_now = 0;

void Position::Init(void)
{
    time_prev = millis();
    x = 0;
    y = 0;
    theta = 0;
}

void Position::Stop(void)
{
    time_prev = millis();
    x = 0; 
    y = 0;
    theta = 0;
}

Position::pose_data Position::ReadPose(void)
{
    return {x,y,theta};
}

void Position::PrintPose(void)
{
    Serial.print(x);
    Serial.print('\t');
    Serial.print(y);
    Serial.print('\t');
    Serial.println(theta);
}

void Position::UpdatePose(float target_speed_left, float target_speed_right)
{
    time_now = millis();
    if(time_now - time_prev >= 50) //update every 50ms for practical reasons
    {
        float dt = ((float)(time_now - time_prev)) / 1000;

        float vl = RomiEncoders.ReadVelocityLeft();
        float vr = RomiEncoders.ReadVelocityRight();

        if(abs(vl - vr) < .05f) {
            x += vl * cos(theta) * dt;
            y += vl * sin(theta) * dt;
        }
        else {
            float w = (vr - vl) / l; //rad/s

            float R = l * .5f * (vr + vl) / (vr - vl); //mm

            x += -R * sin(theta) + R * sin(theta + w * dt);
            y +=  R * cos(theta) - R * cos(theta + w * dt);
            theta += w * dt;
        }

        PrintPose();
        time_prev = time_now;
    }
}

