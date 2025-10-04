#include <LibRobus.h>

#define FULL_SPEED 0.3
#define LOW_SPEED 0.1
#define MAX_PID 105
#define LOW_PID 0

// === PID Constants ===
float kp = 1.0;
float ki = 0.5;
float kd = 0.3;

// === PID State Variables ===
float targetSpeedA = 3200;  // ticks per interval
float targetSpeedB = 3200;

float errorA = 0, lastErrorA = 0, integralA = 0, outputA = 0;
float errorB = 0, lastErrorB = 0, integralB = 0, outputB = 0;

unsigned long lastPIDTime = 0;
const unsigned long PID_INTERVAL = 100;  // milliseconds

int currentCountA = 0;
int currentCountB = 0;

// === Encoder Tracking ===
long lastCountA = 0;
long lastCountB = 0;

// === Replace with your actual motor driver control ===
void setMotorRight(float pwm) {
  MOTOR_SetSpeed(RIGHT, constrain(map(pwm, LOW_PID, MAX_PID, LOW_SPEED, FULL_SPEED), LOW_SPEED, FULL_SPEED));
  Serial.print("Right : "); Serial.println(constrain(map(pwm, LOW_PID, MAX_PID, LOW_SPEED, FULL_SPEED), LOW_SPEED, FULL_SPEED));
}

void setMotorLeft(float pwm) {
  Serial.print("pwm left : "); Serial.println(pwm);
  MOTOR_SetSpeed(LEFT,  constrain(map(pwm, LOW_PID, MAX_PID, LOW_SPEED, FULL_SPEED), LOW_SPEED, FULL_SPEED));
  Serial.print("Left : "); Serial.println(constrain(map(pwm, LOW_PID, MAX_PID, LOW_SPEED, FULL_SPEED), LOW_SPEED, FULL_SPEED));
}

// === PID Update Function ===
void updateDualPID() {

  // --- Read encoder values ---
  currentCountA = ENCODER_Read(RIGHT);
  currentCountB = ENCODER_Read(LEFT);

  long speedA = currentCountA - lastCountA;
  long speedB = currentCountB - lastCountB;

  lastCountA = currentCountA;
  lastCountB = currentCountB;

  // --- PID for Motor A ---
  errorA = targetSpeedA - speedA;
  integralA += errorA * (PID_INTERVAL / 1000.0);
  float derivativeA = (errorA - lastErrorA) / (PID_INTERVAL / 1000.0);
  outputA += kp * errorA + ki * integralA + kd * derivativeA;
  lastErrorA = errorA;

  // --- PID for Motor B ---
  errorB = targetSpeedB - speedB;
  integralB += errorB * (PID_INTERVAL / 1000.0);
  float derivativeB = (errorB - lastErrorB) / (PID_INTERVAL / 1000.0);
  outputB += kp * errorB + ki * integralB + kd * derivativeB;
  lastErrorB = errorB;

  // --- Apply motor outputs ---
  setMotorRight(outputA);
  setMotorLeft(outputB);

  // --- Debug Output ---
  Serial.print("A Speed: "); Serial.print(speedA);
  Serial.print(" | OutA: "); Serial.print(outputA);
  Serial.print(" || B Speed: "); Serial.print(speedB);
  Serial.print(" | OutB: "); Serial.println(outputB);

}

void setup(){
  BoardInit();

  ENCODER_ReadReset(RIGHT);
  ENCODER_ReadReset(LEFT);
}

// === Loop Handler ===
void loop() {
  if (millis() - lastPIDTime >= PID_INTERVAL) 
  {
    lastPIDTime = millis();
    updateDualPID();
  }
}
