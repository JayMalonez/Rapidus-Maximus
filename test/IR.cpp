#include <LibRobus.h>

//Peut etre n'importe quelle patte analogique
#define IR_1_PIN A6
#define IR_2_PIN A7

void setup() {
  BoardInit();
}

void loop() {
  //Donne une valeur entre 0 (loin) et 600 (proche)
  //Etre trop proche cree des fausses donnees
  Serial.println(analogRead(IR_1_PIN));
  delay(250);
}