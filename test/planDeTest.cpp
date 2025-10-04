/*
Projet: Le nom du script
Equipe: Votre numero d'equipe
Auteurs: Les membres auteurs du script
Description: Breve description du script
Date: Derniere date de modification
*/

/*
Inclure les librairies de functions que vous voulez utiliser
*/
#include <LibRobus.h>

/*
Variables globales et defines
 -> defines...
 -> L'ensemble des fonctions y ont acces
*/

#define FULL_TURN_PULSE 3200

bool bumperArr;
int vertpin = 41;
int rougepin = 39;
bool vert = false;
bool rouge = false;
int etat = 0; // = 0 arrêt 1 = avance 2 = recule 3 = TourneDroit 4 = TourneGauche
int etatPast = 0;
float vitesse = 0.30;

int pulseNbMain = 0;
int pulseNbSub = 0;
float ampliSub = 1;

float slowAccelRight = 0;
float slowAccelLeft = 0;

void beep(int count){
  for(int i=0;i<count;i++){
    AX_BuzzerON();
    delay(100);
    AX_BuzzerOFF();
    delay(100);  
  }
  delay(400);
}

void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
};

void avance(){
  MOTOR_SetSpeed(RIGHT, vitesse);
  MOTOR_SetSpeed(LEFT, ampliSub*vitesse);
};

void avanceV2(int nbPulse){
  Serial.print(ampliSub*vitesse);
  ENCODER_ReadReset(RIGHT);
  ENCODER_ReadReset(LEFT);
  do {
    slowAccelRight = ((float)(nbPulse/2 - abs(ENCODER_Read(RIGHT) - nbPulse/2)) / (float)(nbPulse)) + 0.1;
    slowAccelLeft =  (((float)(nbPulse/2 - abs(ENCODER_Read(LEFT)  - nbPulse/2)) / (float)(nbPulse)) + 0.1) * 1.03;
    MOTOR_SetSpeed(RIGHT, slowAccelRight);
    MOTOR_SetSpeed(LEFT,  slowAccelLeft );
    //Serial.println(slowAccelRight);
  }
  while(abs(ENCODER_Read(RIGHT)) < nbPulse);
}

void recule(){
  MOTOR_SetSpeed(RIGHT, -vitesse);
  MOTOR_SetSpeed(LEFT, ampliSub*-vitesse);
};

void tourneDroit(){
  MOTOR_SetSpeed(RIGHT, vitesse);
  MOTOR_SetSpeed(LEFT, ampliSub*-vitesse);
};

void tourneGauche(){
  MOTOR_SetSpeed(RIGHT, -vitesse);
  MOTOR_SetSpeed(LEFT, ampliSub*vitesse);
};

void calibrate(int timeToTestInSec){

  ENCODER_ReadReset(RIGHT);
  ENCODER_ReadReset(LEFT);

  MOTOR_SetSpeed(RIGHT, 0.3);
  MOTOR_SetSpeed(LEFT, 0.3);
  delay(timeToTestInSec * 1000);

  Serial.print("Right : "); Serial.println(ENCODER_Read(RIGHT));
  Serial.print("Left : ");  Serial.println(ENCODER_Read(LEFT));
  ampliSub = (float)ENCODER_Read(RIGHT) / (float)ENCODER_Read(LEFT);
  Serial.print("Ampli (RIGHT/LEFT): ");
  Serial.println(String(ampliSub, 6));
  arret();
  delay(100);

}

void setup(){
  BoardInit();
  
  //initialisation
  pinMode(vertpin, INPUT);
  pinMode(rougepin, INPUT);
  delay(100);
  //calibrate(3);
  beep(2);
}

void loop() {

  if(AX_IsBumper(3)){
    delay(100);
    ENCODER_ReadReset(RIGHT);
    ENCODER_ReadReset(LEFT);
    avance();
    do
    {
      delay(500);
      Serial.print("Right : "); Serial.println(ENCODER_Read(RIGHT));
      Serial.print("Left : ");  Serial.println(ENCODER_Read(LEFT));
      Serial.print("Erreur du gauche-droite"); Serial.println(ENCODER_Read(RIGHT) - ENCODER_Read(LEFT));
      ENCODER_ReadReset(RIGHT);
      ENCODER_ReadReset(LEFT);
    } while (!AX_IsBumper(3));

    arret();

    delay(100);
  }
  else if(AX_IsBumper(1)){
    ENCODER_ReadReset(RIGHT);
    ENCODER_ReadReset(LEFT);
    MOTOR_SetSpeed(RIGHT, vitesse);
    MOTOR_SetSpeed(LEFT,  vitesse);
    delay(3000);
    arret();
    Serial.print("Right : "); Serial.println(ENCODER_Read(RIGHT));
    Serial.print("Left : ");  Serial.println(ENCODER_Read(LEFT));
  }
  else if(AX_IsBumper(2)){
    calibrate(3);
  }

}
