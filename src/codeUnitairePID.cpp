
#include <LibRobus.h>

#define FULL_TURN_PULSE 3200
#define QUARTER_ROTATION_PULSE 2133 //3200 * 2 / 3

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


void tourne(char dir){
  bool leftMotorDone = false;
  bool rightMotorDone = false;
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
    //Serial.print("Right : "); Serial.println(ENCODER_Read(RIGHT));
    //Serial.print("Left : ");  Serial.println(ENCODER_Read(LEFT));
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

  Serial.print("Calibrating for "); Serial.print(timeToTestInSec); Serial.println(" seconds");

  ENCODER_ReadReset(RIGHT);
  ENCODER_ReadReset(LEFT);

  MOTOR_SetSpeed(RIGHT, vitesse);
  MOTOR_SetSpeed(LEFT, vitesse);
  delay(timeToTestInSec * 1000);

  Serial.print("Right : "); Serial.println(ENCODER_Read(RIGHT));
  Serial.print("Left : ");  Serial.println(ENCODER_Read(LEFT));
  ampliSub = (float)ENCODER_Read(RIGHT) / (float)ENCODER_Read(LEFT);
  Serial.print("Ampli (RIGHT/LEFT): ");
  Serial.println(String(ampliSub, 6));
  Serial.println();  
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

  BoardInit();
  beep(3);

  calibrate(5);
  delay(1000);
  beep(2);
}

void loop() {

  if(AX_IsBumper(3))
  {
    while (AX_IsBumper(3))
      delay(1);
    beep(2);

    ENCODER_ReadReset(RIGHT);
    ENCODER_ReadReset(LEFT);

    avance();
    do
    {
      delay(100);
      if (DEBUG){
        Serial.print("Right : "); Serial.println(ENCODER_Read(RIGHT));
        Serial.print("Left : ");  Serial.println(ENCODER_Read(LEFT));
        Serial.print("Erreur du Droite-Gauche : "); Serial.println(ENCODER_Read(RIGHT) - ENCODER_Read(LEFT));
        Serial.print("Vitesse gauche avant "); Serial.println(String(vitesse*ampliSub, 6));
        //Serial.print("Erreur Droite/Gauche pow 2: "); Serial.println(String(((float)(pow(ENCODER_Read(RIGHT), 1)) / (float)(pow(ENCODER_Read(LEFT), 1)), 6)));
        Serial.print("AmpliSub avant : "); Serial.println(String(ampliSub, 6));
      }

      ampliSub *= (float)(pow(ENCODER_Read(RIGHT), 2)) / (float)(pow(ENCODER_Read(LEFT), 2));
      if (DEBUG){
        Serial.print("AmpliSub apres : "); Serial.println(String(ampliSub, 6));
        Serial.print("Vitesse gauche apres : "); Serial.println(String(vitesse*ampliSub, 6));
        Serial.println();
      }
      ENCODER_ReadReset(RIGHT);
      ENCODER_ReadReset(LEFT);
      avance();
    } while (!AX_IsBumper(3));

    arret();
    delay(500);
    Serial.println("End");  
    beep(1);
  }
  else if (AX_IsBumper(2))
  {
    tourne('R');
    tourne('L');
  }
  
}
