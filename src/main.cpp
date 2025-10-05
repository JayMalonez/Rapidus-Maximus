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
  parcours[0][0][0] = 0;
  parcours[0][1][0] = 0; //Arrivée
  parcours[0][2][0] = 0;

  //bordure droite
  for (int i = 0; i < 10; i++) {
    parcours[i][2][1] = 1;
  }
  
  //bordure du bas
  parcours[9][0][2] = 1;
  parcours[9][1][2] = 1; //Départ
  parcours[9][2][2] = 1;

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
#define QUARTER_ROTATION_PULSE 2000 //2133 //3200 * 2 / 3


#define DEBUG false

bool bumperArr;
int vertpin = 41;
int rougepin = 39;
bool vert = false;
bool rouge = false;
int etat = 0; // = 0 arrêt 1 = avance 2 = recule 3 = TourneDroit 4 = TourneGauche
int etatPast = 0;
float vitesse = 0.3; //0,30

int pulseNbMain = 0;
int pulseNbSub = 0;
float ampliSub = 1;
float slowAccelRight = 0;
float slowAccelLeft = 0;

const int pin5KHZ = 10;
const int pinAmbiant = 11;

const int seuilAsbolu = 1;
const int seuilRelatif = 1;
bool firstRun = true;

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

void avance(int nbPulse){
  ENCODER_ReadReset(RIGHT);
  ENCODER_ReadReset(LEFT);
  do {
    slowAccelRight = ((float)(nbPulse/2 - abs(ENCODER_Read(RIGHT) - nbPulse/2)) / (float)(nbPulse)) + 0.1;
    slowAccelLeft =  ((float)(nbPulse/2 - abs(ENCODER_Read(LEFT)  - nbPulse/2)) / (float)(nbPulse)) + 0.1;
    MOTOR_SetSpeed(RIGHT, slowAccelRight);
    MOTOR_SetSpeed(LEFT,  slowAccelLeft );
    //Serial.println(slowAccelRight);
  }
  while(abs(ENCODER_Read(RIGHT)) < nbPulse);
};

/*void avance50(){
  ENCODER_Reset(RIGHT);
  ENCODER_Reset(LEFT);
;

  while (true){
    long droite = ENCODER_Read(RIGHT);
    long gauche = ENCODER_Read(LEFT);
    long moyenne = (droite + gauche) / 2;

    if (moyenne >= 3800)
    {
      break;
    }

    avance();
  }
  delay(1000);
};*/

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

void setup() {
  BoardInit();
  initParcours();
  beep(3);

  pinMode(pin5KHZ, INPUT);
  pinMode(pinAmbiant, INPUT);
  firstRun = true;

  //calibrate(5);
  delay(1000);
  beep(2);
  delay(5000);

  //case de départ
  currentTile[0] = 9;
  currentTile[1] = 1;

  
}

void loop() {
  
  if (firstRun){
    
    while(detecteSifflet() == false){
      
    }

    firstRun = false;
  }

  if (currentTile[0] == -1) {
    Serial.println("Arrêt de l'algorithme");
    Serial.print(currentTile[0]);
    Serial.print(", ");
    Serial.print(currentTile[1]);
    arret();
    return; 
  }

  //regarde si on peut Avancer
  if (parcours[currentTile[0]][currentTile[1]][0] == 0 && (digitalRead(vertpin) == 1 || digitalRead(rougepin) == 1)) {
    avance(6250);
    currentTile[0]--;

    Serial.print(currentTile[0]);
    Serial.print(", ");
    Serial.print(currentTile[1]);
  //sinon vérifie à droite
  } else if (parcours[currentTile[0]][currentTile[1]][1] == 0)
  {
    tourne('R');
    delay(1000);
    arret();

    if (digitalRead(vertpin) == 0 || digitalRead(rougepin) == 0)
    {
      tourne('L');
      delay(400);
      tourne('L');
      delay(400);
      avance(6250);
      delay(400);
      tourne('R'); //se replace dans la bonne direction
      delay(1000);
    } else {
      avance(6250);
      delay(400);
      tourne('L');
      delay(1000);

      if (currentTile[1] == 0 && (digitalRead(vertpin) == 0 || digitalRead(rougepin) == 0))
      {
        Serial.print("continue");
        tourne('L');
        delay(400);
        avance(6250);
        delay(400);
        avance(6250);
        tourne('R');
        delay(400);
        currentTile[1]--;
      } else
      {
        tourne('L');
        currentTile[1]++;
      }

      Serial.print(currentTile[0]);
      Serial.print(", ");
      Serial.print(currentTile[1]);
    }
  //sinon vérifie à gauche
  } else if (parcours[currentTile[0]][currentTile[1]][3] == 0)
  {
    tourne('L');
    avance(6250);
    currentTile[1]--;

    Serial.print(currentTile[0]);
    Serial.print(", ");
    Serial.print(currentTile[1]);
    tourne('R');
  }
  delay(400);
  arret();
  delay(500);
}