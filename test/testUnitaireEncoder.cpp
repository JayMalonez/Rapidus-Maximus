#include <LibRobus.h>

void setup(){
  BoardInit();
  
  MOTOR_SetSpeed(RIGHT, 0.3);
  MOTOR_SetSpeed(LEFT, 0.3);
}

void loop() {
  delay(500);
  //Serial.print("Right : "); Serial.println(ENCODER_Read(RIGHT));
  Serial.print("Left : ");  Serial.println(ENCODER_Read(LEFT));
  Serial.print("Erreur du gauche-droite : "); Serial.println(ENCODER_Read(RIGHT) - ENCODER_Read(LEFT));
  ENCODER_ReadReset(RIGHT);
  ENCODER_ReadReset(LEFT);
}
