#include <LibRobus.h>

/* 
matrice de 3 lignes par 10 colonnes contenant des tableaux de 4 représentant une case 
[haut, droite, bas, gauche]
*/
int parcours[10][3][4];
//case dans laquelle se trouve le robot [ligne, colonne]
int currentTile[2];

//initialise les bordures dans le parcours
void initParcours() {
  // tableau vide
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 3; j++) {
      parcours[i][j][0] = 0; // haut
      parcours[i][j][1] = 0; // droite
      parcours[i][j][2] = 0; // bas
      parcours[i][j][3] = 0; // gauche
    }
  }

  //bordure du haut
  parcours[0][0][0] = 1;
  parcours[0][1][0] = 0; //Arrivée
  parcours[0][2][0] = 1;

  //bordure droite
  for (int i = 0; i < 10; i++) {
    parcours[i][2][1] = 1;
  }
  
  //bordure du bas
  parcours[9][0][3] = 1;
  parcours[9][1][3] = 1; //Départ
  parcours[9][2][3] = 1;

  //bordure gauche
  for (int i = 0; i < 10; i++) {
    parcours[i][0][3] = 1;
  }

  //bordure du milieu lignes : 0,2,4,6,8
  parcours[0][1][1] = 1;
  parcours[0][1][3] = 1;
  parcours[2][1][1] = 1;
  parcours[2][1][3] = 1;
  parcours[4][1][1] = 1;
  parcours[4][1][3] = 1;
  parcours[6][1][1] = 1;
  parcours[6][1][3] = 1;
  parcours[8][1][1] = 1;
  parcours[8][1][3] = 1;
}

#define FULL_TURN_PULSE 3200
#define QUARTER_ROTATION_PULSE 2133 //3200 * 2 / 3

#define DEBUG false

bool bumperArr;
int vertpin = 41;
int rougepin = 39;
bool vert = false;
bool rouge = false;
int etat = 0; // = 0 arrêt 1 = avance 2 = recule 3 = TourneDroit 4 = TourneGauche
int etatPast = 0;
float vitesse = 0.2; //0,30

int pulseNbMain = 0;
int pulseNbSub = 0;
float ampliSub = 1;

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
  Serial.println("Avance");
  MOTOR_SetSpeed(RIGHT, constrain(vitesse, 0.1, 0.5));
  MOTOR_SetSpeed(LEFT, constrain(ampliSub*vitesse, 0.1, 0.5));
};

void avance50(){
  ENCODER_Reset(RIGHT);
  ENCODER_Reset(LEFT);
  while (ENCODER_Read(RIGHT) < 3000){
    avance();
  }
  delay(1000);
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
    Serial.println("Tourne gauche 90");
    MOTOR_SetSpeed(RIGHT,vitesse);
    MOTOR_SetSpeed(LEFT, ampliSub*-vitesse);
  }
  else if (dir == 'R'){
    Serial.println("Tourne droite 90");
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

void setup() {
  BoardInit();
  initParcours();
  beep(3);

  calibrate(5);
  delay(1000);
  beep(2);

  //case de départ
  currentTile[0] = 9;
  currentTile[1] = 1;
}

void loop() {
  if (currentTile[0] == 8) {
    Serial.println("Arrêt de l'algorithme");
    Serial.print(currentTile[0]);
    Serial.print(", ");
    Serial.print(currentTile[1]);
    arret();
    return; 
  }

  //regarde si on peut Avancer
  if (parcours[currentTile[0]][currentTile[1]][0] == 0 /*&& digitalRead(vertpin) == 1 && digitalRead(rougepin) == 1*/) {
    avance50();
    currentTile[0]--;

    Serial.print(currentTile[0]);
    Serial.print(", ");
    Serial.print(currentTile[1]);
  //sinon vérifie à droite
  } else if (parcours[currentTile[0]][currentTile[1]][1] == 0)
  {
    tourne('R');
    /*if (digitalRead(vertpin) == 0 && digitalRead(rougepin) == 0)
    {
     tourneGauche90(); //90
     tourneGauche90(); //90
     avance50();
     tourneDroit90(); //se replace dans la bonne direction
     currentTile[1]--;
    } else {
      avance50();
      currentTile[1]++;

      Serial.print(currentTile[0]);
      Serial.print(", ");
      Serial.print(currentTile[1]);
      tourneGauche();
    }*/
  //sinon vérifie à gauche
  } else if (parcours[currentTile[0]][currentTile[1]][3] == 0)
  {
    tourne('L');
    //avance50();
    currentTile[1]--;

    Serial.print(currentTile[0]);
    Serial.print(", ");
    Serial.print(currentTile[1]);
    //tourneDroit90();
  }
  delay(400);
  arret();
  delay(500);
}