#include <Romi32U4.h>
#include "Sonar_sensor.h"

SonarSensor *SONAR;

void Echo() {
    SONAR->echo();
}

void SonarSensor::Update() {
    SONAR->update();
}

void SonarSensor::Init(void)
{
    SONAR = this;

    pinMode(pin_TRIG,OUTPUT);
    digitalWrite(pin_TRIG, LOW);
    pinMode(pin_ECHO, INPUT);

    attachInterrupt(3, Echo, CHANGE);
    ping();
}

void SonarSensor::PrintData(void)
{
    Serial.println(ReadData());
}

float SonarSensor::ReadData(void)
{
    //assignment 1.2
    //read out and calibrate your sonar sensor, to convert readouts to distance in [cm]
    return distance;
}



/**
 * @brief Sends the command to ping to the ultrasonic sensor
*/
void SonarSensor::ping() {
    pinged = true;
    digitalWrite(pin_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(pin_TRIG, LOW);
    sonarWaitStart = micros();
}
/**
 * @brief Interrupt for sonar ping sent and recived
*/
void SonarSensor::echo() {
    if(digitalRead(pin_ECHO)) {
        sonarOut = micros();
        return;
    }
    lastSonarTime = micros() - sonarOut;
    //raw time here
    distance = lastSonarTime * microsToDistance;
    Serial.println(lastSonarTime);
    sonarWaitStart = micros();
    pinged = false;
}

void SonarSensor::update() {
    if(pinged) {
        if(sonarOut + timeoutPeriod <= micros()) ping();
    } else {
        if(sonarWaitStart + waitPeriod <= micros()) ping();
    }
}