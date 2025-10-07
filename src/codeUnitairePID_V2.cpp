//
//La distance par rapport au temps est égale à 7.36 cm/1000 pulses 50 cm / x pulses 


#include <LibRobus.h>

#define FULL_TURN_PULSE 3200
#define QUARTER_ROTATION_PULSE 2133 //3200 * 2 / 3

#define BACK_BUTTON 3

#define DEBUG false

float vitesse = 0.2;

int pulseNbMain = 0;
int pulseNbSub = 0;
float ampliSub = 1;



void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
};

void avance(){
  MOTOR_SetSpeed(RIGHT, constrain(vitesse, 0.1, 0.5));
  MOTOR_SetSpeed(LEFT, constrain(ampliSub*vitesse, 0.1, 0.5));
};

void avancePID(int intervalEnMS){

  //Reset les encoders pour la prochaine lecture
  ENCODER_ReadReset(RIGHT);
  ENCODER_ReadReset(LEFT);

  //Applique le nouveau ampliSub aux moteurs
  MOTOR_SetSpeed(RIGHT, constrain(vitesse, 0.1, 0.5));
  MOTOR_SetSpeed(LEFT, constrain(ampliSub*vitesse, 0.1, 0.5));

  delay(intervalEnMS);
  //Print des encoders Droite et Gauche AVANT le ratio applique au moteur de gauche
  if (DEBUG){
    Serial.print("Right : "); Serial.println(ENCODER_Read(RIGHT));
    Serial.print("Left : ");  Serial.println(ENCODER_Read(LEFT));
    Serial.print("Erreur du Droite-Gauche : "); Serial.println(ENCODER_Read(RIGHT) - ENCODER_Read(LEFT));
    Serial.print("Vitesse gauche avant "); Serial.println(String(vitesse*ampliSub, 6));
    //Serial.print("Erreur Droite/Gauche pow 2: "); Serial.println(String(((float)(pow(ENCODER_Read(RIGHT), 1)) / (float)(pow(ENCODER_Read(LEFT), 1)), 6)));
    Serial.print("AmpliSub avant : "); Serial.println(String(ampliSub, 6));
  }

  //Divise le nb de pulse de l'encoder Droite par le nb de pulse de l'encoder Gauche pour trouver le ratio qu'il appliquer au moteur gauche
  //La valeur des encoders est puissance pour que le robot subit une surcorrection si la difference entre les encoders est tres haute ou tres basse
  //Ce quotient est ensuite multiplié par le ratio d'avant
  ampliSub *= (float)(pow(ENCODER_Read(RIGHT), 2)) / (float)(pow(ENCODER_Read(LEFT), 2));

  //Print des encoders Droite et Gauche APRES le ratio applique au moteur de gauche
  if (DEBUG){
    Serial.print("AmpliSub apres : "); Serial.println(String(ampliSub, 6));
    Serial.print("Vitesse gauche apres : "); Serial.println(String(vitesse*ampliSub, 6));
    Serial.println();
  }
 
};

void tourne(char dir){

  bool leftMotorDone = false;
  bool rightMotorDone = false;

  if (DEBUG)
    Serial.print("Enter tourne Gauche");

  arret();
  delay(100);

  ENCODER_ReadReset(RIGHT);
  ENCODER_ReadReset(LEFT);

  if (dir == 'L'){
    MOTOR_SetSpeed(RIGHT,vitesse);
    MOTOR_SetSpeed(LEFT, ampliSub*-vitesse);
  }
  else if (dir == 'R'){
    MOTOR_SetSpeed(RIGHT,-vitesse);
    MOTOR_SetSpeed(LEFT, ampliSub*vitesse);
  }

  while(!rightMotorDone && !leftMotorDone)
  {
    if (DEBUG){
      Serial.print("Right : "); Serial.println(ENCODER_Read(RIGHT));
      Serial.print("Left : ");  Serial.println(ENCODER_Read(LEFT));
    }

    //Si l'encoder detecte que la roue a fait le nb de pulse necessaire pour un demi tour, il arrete
    //Cette boucle se termine seulement si les deux encoders on fait le nombre necessaire de pulse
    if (abs(ENCODER_Read(RIGHT)) >= QUARTER_ROTATION_PULSE)
    {
      MOTOR_SetSpeed(RIGHT, 0);
      rightMotorDone = true;
    }
    if (abs(ENCODER_Read(LEFT)) >= QUARTER_ROTATION_PULSE)
    {
      MOTOR_SetSpeed(LEFT, 0);
      leftMotorDone = true;
    }
  }
  
  arret();
  delay(100);
};

void calibrate(int timeToTestInSec){

  if (DEBUG)
    Serial.print("Calibrating for "); Serial.print(timeToTestInSec); Serial.println(" seconds");

  ENCODER_ReadReset(RIGHT);
  ENCODER_ReadReset(LEFT);

  MOTOR_SetSpeed(RIGHT, vitesse);
  MOTOR_SetSpeed(LEFT, vitesse);

  delay(timeToTestInSec * 1000);

  if (DEBUG){
    Serial.print("Right : "); Serial.println(ENCODER_Read(RIGHT));
    Serial.print("Left : ");  Serial.println(ENCODER_Read(LEFT));
  }

  //Divise le nb de pulse de l'encoder Droite par le nb de pulse de l'encoder Gauche pour trouver le ratio qu'il appliquer au moteur gauche
  ampliSub = (float)ENCODER_Read(RIGHT) / (float)ENCODER_Read(LEFT);

  if (DEBUG){
    Serial.print("Ampli (RIGHT/LEFT): ");
    Serial.println(String(ampliSub, 6));
    Serial.println();  
  }
  arret();
  delay(100);
}

void beep(int count){
  for(int i=0;i<count;i++){
    AX_BuzzerON();
    delay(100);
    AX_BuzzerOFF();
    delay(100);  
  }
  delay(400);
}

void setup(){
  //Init de la librairie Robus
  BoardInit();
  beep(3);

  //Calibration du robot sans amplification au moteur de gauche pour avoir une valeur aproximative
  //Le code dans le loop peut ensuite faire de petit changement sur cette amplification
  calibrate(5);

  delay(1000);
  beep(2);
}

void loop() {

  //Il faut appuyer sur le bouton et le relacher pour faire avancer le robot
  //Pour l'arreter, il faut maintenir le bouton jusqu'à qu'il arrete
  if(AX_IsBumper(BACK_BUTTON))
  {
    while (AX_IsBumper(BACK_BUTTON))
      delay(1);
    beep(2);

    do {
      avancePID(100);
    } while (!AX_IsBumper(BACK_BUTTON));

    arret();
    delay(500);  
    beep(1);
  }
  else if (AX_IsBumper(2))
  {
    tourne('R'); //Right
    tourne('L'); //Left
  }
}
