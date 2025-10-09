#include <LibRobus.h>

//LE SERVO PEUT SEULEMENT BOUGER DE 0 A 180 DEGREE
//DEUX OBJET "Servo" DANS UNE LISTE "servo[2]" EXISTE DEJA

//Doivent etre des pattes PWM (Digital IO pins 2 – 13 and 44 – 46)
#define SERVOMOTOR_1_PIN 44
#define SERVOMOTOR_2_PIN 45

int oriServo = 0;


void setup() {
  BoardInit();

  servo[0].attach(SERVOMOTOR_1_PIN);  // attaches the servo on pin 9 to the servo objectư
  servo[0].write(0);   // rotate slowly servo to 0 degrees immediately

}

void loop() {

  servo[0].write(oriServo);                  // sets the servo position according to the scaled value

  delay(10);
  //Augmente de 1 jusqu'a 180 puis revient a 0
  oriServo++;
  oriServo = oriServo % 181; 

}