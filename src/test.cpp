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
  MOTOR_SetSpeed(RIGHT,vitesse);
  MOTOR_SetSpeed(LEFT, ampliSub*vitesse);
};

void avanceV2(int nbPulse){
  ENCODER_ReadReset(RIGHT);
  ENCODER_ReadReset(LEFT);
  do {
    slowAccelRight = ((float)(nbPulse/2 - abs(ENCODER_Read(RIGHT) - nbPulse/2)) / (float)(nbPulse)) + 0.1;
    slowAccelLeft =  ((float)(nbPulse/2 - abs(ENCODER_Read(LEFT)  - nbPulse/2)) / (float)(nbPulse)) + 0.1;
    MOTOR_SetSpeed(RIGHT, slowAccelRight);
    MOTOR_SetSpeed(LEFT,  slowAccelLeft );
    Serial.println(slowAccelRight);
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

void calibrate(int nbOfMeasure){

  ENCODER_ReadReset(RIGHT);
  ENCODER_ReadReset(LEFT);
  do {
    avance();
  }
  while(ENCODER_Read(RIGHT) < FULL_TURN_PULSE*nbOfMeasure);

  pulseNbSub = ENCODER_Read(LEFT);

  ampliSub = ((float)FULL_TURN_PULSE * nbOfMeasure) / (float)pulseNbSub;
  Serial.print("Ampli : ");
  Serial.println(ampliSub);
  arret();
  delay(100);
  beep(2);
}

void setup(){
  BoardInit();
  
  //initialisation
  pinMode(vertpin, INPUT);
  pinMode(rougepin, INPUT);
  delay(100);
  //calibrate(2);
}

void loop() {
  if(AX_IsBumper(1))
  {
    ENCODER_ReadReset(RIGHT);
    ENCODER_ReadReset(LEFT);
    do {
      tourneDroit();
    }
    while(abs(ENCODER_Read(RIGHT)) < FULL_TURN_PULSE);
    Serial.println(ENCODER_Read(LEFT));
    
    arret();
    delay(500);

    ENCODER_ReadReset(RIGHT);
    ENCODER_ReadReset(LEFT);
    do {
      tourneGauche();
    }
    while(abs(ENCODER_Read(RIGHT)) < FULL_TURN_PULSE);
    Serial.println(ENCODER_Read(LEFT));
    arret();
    delay(500);
  }
  else if(AX_IsBumper(3)){
    avanceV2(10000);
    /*ENCODER_ReadReset(RIGHT);
    ENCODER_ReadReset(LEFT);
    do {

      
      slowAccelRight = ((float)(FULL_TURN_PULSE/2 - abs(ENCODER_Read(RIGHT) - FULL_TURN_PULSE/2)) / (float)(FULL_TURN_PULSE)) + 0.1;
      slowAccelLeft =  ((float)(FULL_TURN_PULSE/2 - abs(ENCODER_Read(LEFT)  - FULL_TURN_PULSE/2)) / (float)(FULL_TURN_PULSE)) + 0.1;
      MOTOR_SetSpeed(RIGHT, slowAccelRight);
      MOTOR_SetSpeed(LEFT,  slowAccelLeft );
      Serial.println(slowAccelRight);
    }
    while(abs(ENCODER_Read(RIGHT)) < FULL_TURN_PULSE);*/
  }
  else
  {
    arret();
    delay(100);
  }
/*
  Serial.println(ENCODER_Read(RIGHT));
  Serial.println(ENCODER_Read(LEFT));
  if(AX_IsBumper(1))
    {
      ENCODER_ReadReset(RIGHT);
      ENCODER_ReadReset(LEFT);
    }
  delay(100);
*/
}
