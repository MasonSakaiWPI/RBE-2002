#include <Arduino.h>
#include <Romi32U4.h>
#include "openmv.h"
#include "apriltagdatum.h"
# include <stdio.h>

int serial_putc(char c, FILE *) 
{
  Serial.write(c);

  return c;
} 

OpenMV camera;
AprilTagDatum tag;
Romi32U4Motors motors;

const float desiredDistance = 20; //cm
const float kp_dist = 30, kd_dist = 15;
const float tagDistRatio = 20/40; //cm per pixel

const int desiredPixX = 60;
const float kp_turn = 50, kd_turn = 25;

// const float tagRealWidth = 4.4f; //cm
// const float tagTargetPxWidth = 40;
const float radPerPixel = -1; //get value

//width at 20cm: 40px
//4.4cm real width

/** Calculates the distance to the tag. Assumes 20 cm/40 pixels
 * @param tag april tag data
 * @return the distance to the tag in cm
 */
float DistanceToTag(AprilTagDatum& tag){
    float tagReadjustedW = 0.0;
    float theDistanceToTag = 0.0;
    float tagRotationRadians = 0.0;
    
    tagRotationRadians = tag.rot*PI/180;
    tagReadjustedW = tag.h / (abs(cos(tagRotationRadians)) + abs(sin(tagRotationRadians)));
    //If tagReadjustedW is twice 40, the distance to the sensor is half of 20.
    //If tagReadjustedW is half 40, the distance to the sensor is twice of 20
    //At distance of 40 cm, should be 20 pixels wide
    theDistanceToTag = 20*(40/tagReadjustedW); // recheck later
    return theDistanceToTag;
}

float lastDist, lastTurn;

void controlRobot(float distanceToTag, float radsFromCenter) {

  float e_dist = distanceToTag - desiredDistance;
  if(e_dist > 20) e_dist = 20;
  else if(e_dist < -20) e_dist = -20;

  float E_dist = e_dist - lastDist;
  lastDist = e_dist;
  float u_dist = kp_dist * e_dist + kd_dist * E_dist;

  float e_turn = radsFromCenter;
  float E_turn = e_turn - lastTurn;
  lastTurn = e_turn;
  float u_turn = kp_turn * e_turn + kd_turn * E_turn;

  float u_left = u_dist - u_turn;
  float u_right = u_dist + u_turn;

  //printf("dist: %8.4f, %8.4f, %8.4f, %8.4f\n", distanceToTag, e_dist, E_dist, u_dist);
  //printf("turn: %8.4f, %8.4f, %8.4f, %8.4f\n", radsFromCenter, e_turn, E_turn, u_turn);
  //printf("effs: %8.4f, %8.4f, %8.4f, %8.4f\n\n", u_dist, u_turn, u_left, u_right);

  Serial.println();

  motors.setEfforts(u_left,u_right);
}

void setup() {
  Wire.begin();
  Wire.setClock(100000ul);

  Serial.begin(7200);
  fdevopen(&serial_putc, 0);

  motors.init();
}

long lastSeen = 0;

void loop() {

  if(camera.getTagCount() > 0) {
    camera.readTag(tag);

    float dist = DistanceToTag(tag);
    float turn = 0;//(tag.cx - 60);

    controlRobot(dist, turn);

    lastSeen = millis();
    //delay(100);
    
    // Serial.print("Header: ");
    // Serial.println(tag.header);
    // Serial.print("ID: ");
    // Serial.println(tag.id);
    // Serial.print("rot: ");
    // Serial.println(tag.rot); //Degrees
    // Serial.print("cx: ");
    // Serial.println(tag.cx);
    // Serial.print("cy: ");
    // Serial.println(tag.cy);
    // Serial.print("w: ");
    // Serial.println(tag.w);
    // Serial.print("h: ");
    // Serial.println(tag.h);
    // Serial.println();

    // delay(1000);
  }
  else if(millis() - lastSeen > 100) {
    motors.setEfforts(0, 0);
  }

}
