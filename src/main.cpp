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

void printParcours() {
  for (int i = 0; i < 10; i++) {
    Serial.print("Ligne "); Serial.println(i);
    for (int j = 0; j < 3; j++) {
      Serial.print("  Case("); Serial.print(i);
      Serial.print(","); Serial.print(j);
      Serial.print(") -> [");

      for (int d = 0; d < 4; d++) {
        Serial.print(parcours[i][j][d]);
        if (d < 3) Serial.print(",");
      }
      Serial.println("]"); // fin des directions
    }
  }
}

#define FULL_TURN_PULSE 3200
#define DISTANCE 1000

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
  Serial.println("Avance");
  MOTOR_SetSpeed(RIGHT,vitesse);
  MOTOR_SetSpeed(LEFT, ampliSub*vitesse);
  arret();
};


void avance50(){
  ENCODER_Reset(RIGHT);
  while (ENCODER_Read(RIGHT) < 6560){
    avance();
  }
  delay(1000);
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
  Serial.println("Tourne droite 90");
  MOTOR_SetSpeed(RIGHT, -vitesse);
  MOTOR_SetSpeed(LEFT, ampliSub*vitesse);
  delay(1000);
  arret();
};

void tourneGauche(){
  Serial.println("Tourne gauche 90");
  MOTOR_SetSpeed(RIGHT, vitesse);
  MOTOR_SetSpeed(LEFT, ampliSub*-vitesse);
  delay(1000);
  arret();
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

void setup() {
  BoardInit();
  initParcours();
  //printParcours();

  pinMode(vertpin, INPUT);
  pinMode(rougepin, INPUT);
  delay(100);

  //case de départ
  currentTile[0] = 9;
  currentTile[1] = 1;
}

void loop() {
  if (currentTile[0] == -1) {
    Serial.println("Arrêt de l'algorithme");
    Serial.print(currentTile[0]);
    Serial.print(", ");
    Serial.print(currentTile[1]);
    arret();
    return; 
  }

  //regarde si on peut Avancer
  if (parcours[currentTile[0]][currentTile[1]][0] == 0 && digitalRead(vertpin) == 1 && digitalRead(rougepin) == 1) {
    avance50();
    currentTile[0]--;

    Serial.print(currentTile[0]);
    Serial.print(", ");
    Serial.print(currentTile[1]);
  //sinon vérifie à droite
  } else if (parcours[currentTile[0]][currentTile[1]][1] == 0)
  {
    tourneDroit();
    if (digitalRead(vertpin) == 0 && digitalRead(rougepin) == 0)
    {
     tourneGauche(); //90
     tourneGauche(); //90
     avance50();
     tourneDroit(); //se replace dans la bonne direction
     currentTile[1]--;
    } else {
      avance50();
      currentTile[1]++;

      Serial.print(currentTile[0]);
      Serial.print(", ");
      Serial.print(currentTile[1]);
      tourneGauche();
    }
  //sinon vérifie à gauche
  } else if (parcours[currentTile[0]][currentTile[1]][3] == 0)
  {
    tourneGauche();
    avance50();
    currentTile[1]--;

    Serial.print(currentTile[0]);
    Serial.print(", ");
    Serial.print(currentTile[1]);
    tourneDroit();
  }
  delay(400);
}