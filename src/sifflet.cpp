#include <LibRobus.h>

const int pin5KHZ = 10;
const int pinAmbiant = 11;

const int seuilAsbolu = 1;
const int seuilRelatif = 1;

void setup() {
  BoardInit();
  pinMode(pin5KHZ, INPUT);
  pinMode(pinAmbiant, INPUT);
}

float lecture5KHZ() {
  return (analogRead(pin5KHZ)/1023)*5;
}

float lectureAmbiant() {
  return (analogRead(pinAmbiant)/1023)*5;
}

bool detecteSifflet() {
  float valeur5KHZ = lecture5KHZ();
  float valeurAmbiant = lectureAmbiant();

  if (valeur5KHZ > seuilAsbolu && (valeur5KHZ - valeurAmbiant) > seuilRelatif) {
        return true;
  } 

  else {
        return false;
  }
}