#include <Arduino.h>
#include <Button.h>
#include "Ultrasonic.h"
#include "IR_sensor.h"
#include "IMU.h"


//Define all sensors
Button buttonA(14);
Ultrasonic sonar;
IRsensor ir;
IMU_sensor imu_sensor;


/**
 * sendMessage creates a string of the form
 *      topic:message
 * which is what the corresponding ESP32 code expects.
 * */
void sendMessage(const String& topic, const String& message)
{
    Serial1.println(topic + String(':') + message);
}

/**
 * check the serial 1 for the newline for new information
*/
String serString1;
bool checkSerial1(void)
{
    while(Serial1.available())
    {
        char c = Serial1.read();
        serString1 += c;

        if(c == '\n')
        {
            return true;
        }
    }

    return false;
}

//set up and intialize all sensors 
void setup() 
{
    Serial.begin(115200);
    delay(100);  //give it a moment to bring up the Serial

    sonar.setup();
    sonar.start();
    ir.Init();
    imu_sensor.Init();

    Serial.println("setup()");

    Serial1.begin(115200);
    digitalWrite(0, HIGH); // Set internal pullup on RX1 to avoid spurious signals
    buttonA.init();
    Wire.begin();
    Wire.setClock(100000ul);
    motors.init();

    Serial.println("/setup()");
}

/**
 * This basic example sends the time (from millis()) every
 * five seconds. See the `readme.md` in the root directory of this repo for 
 * how to set up the WiFi. 
 * */
void loop() 
{
    sonar.update();

    static uint32_t lastSend = 0;
    uint32_t currTime = millis();
    if(currTime - lastSend >= 500) //send every five seconds
    {
        lastSend = currTime;
        sendMessage("timer/time", String(currTime)); //send time to mqtt

        sendMessage("sonar/active", String(sonar.isActive())); //send sonar activity status to mqtt
        sendMessage("sonar/distance", String(sonar.getDistance())); //send sonar distance to mqtt

        sendMessage("irsensor", String(ir.ReadData())); //send IR sensor to mqtt
        
        sendMessage("imu/z", String(imu_sensor.ReadAcceleration().Z)); //send IMU sensor to mqtt
    }

    // Check to see if we've received anything
    if(checkSerial1())
    {
        Serial.print("Rec'd:\t");
        Serial.print(serString1);
        serString1 = "";
    }

    // Defaults to just sending one message, but increase the message count
    // if you want to test how fast you can send
    static int msgCountToSend = 0;
    if(buttonA.checkButtonPress()) msgCountToSend = 1;

    while(msgCountToSend)
    {
        sendMessage("button/time", String(currTime + msgCountToSend--));
    }
}