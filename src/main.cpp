#include <LibRobus.h>

/* 
matrice de 3 lignes par 10 colonnes contenant des tableaux de 4 représentant une case 
[haut, droite, bas, gauche]
*/
int parcours[10][3][4];
//case dans laquelle se trouve le robot [ligne, colonne]
int currentTile[2];

//initialise les bordures dans le parcours
void initParcours(int run) {
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

  if (run == 1)
  {
    //bordure du milieu lignes : 0,2,4,6,8
    for (int i = 0; i<= 8; i+=2) {
      parcours[i][1][1] = 1;
      parcours[i][1][3] = 1;
    }
  } else
  {
    //bordure du milieu lignes : 1,3,5,7,9
    for (int i = 1; i<= 9; i+=2) {
      parcours[i][1][1] = 1;
      parcours[i][1][3] = 1;
    }
  }
  
  
  
  
}

#define FULL_TURN_PULSE 3200
#define QUARTER_ROTATION_PULSE 2000 //2133 //3200 * 2 / 3
#define DISTANCE 6600 //nb de pulse (avance)

bool depart = true;
bool bumperArr;
int vertpin = 41;
int rougepin = 39;
bool vert = false;
bool rouge = false;
int etat = 0; // = 0 arrêt 1 = avance 2 = recule 3 = TourneDroit 4 = TourneGauche
int etatPast = 0;
float vitesse = 0.35; //0,30

int pulseNbMain = 0;
int pulseNbSub = 0;
float ampliSub = 1;
float slowAccelRight = 0;
float slowAccelLeft = 0;

const int pin5KHZ = A5;
const int pinAmbiant = A4;

const float seuilAsbolu = 3.5;
const float seuilRelatif = 1.5;
bool firstRun = true;
bool siffletDetecte = false;

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

/*void avance(int nbPulse){
  //Reset les encoders pour la prochaine lecture
  ENCODER_ReadReset(RIGHT);
  ENCODER_ReadReset(LEFT);

  //Applique le nouveau ampliSub aux moteurs
  MOTOR_SetSpeed(RIGHT, constrain(vitesse, 0.1, 0.5));
  MOTOR_SetSpeed(LEFT, constrain(ampliSub*vitesse, 0.1, 0.5));

  while (abs(ENCODER_Read(RIGHT)) < nbPulse)
  {
    //rien
  }
  
  //Print des encoders Droite et Gauche AVANT le ratio applique au moteur de gauche

  //Divise le nb de pulse de l'encoder Droite par le nb de pulse de l'encoder Gauche pour trouver le ratio qu'il appliquer au moteur gauche
  //La valeur des encoders est puissance pour que le robot subit une surcorrection si la difference entre les encoders est tres haute ou tres basse
  //Ce quotient est ensuite multiplié par le ratio d'avant
  ampliSub *= (float)(pow(ENCODER_Read(RIGHT), 2)) / (float)(pow(ENCODER_Read(LEFT), 2));
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
  return (analogRead(pin5KHZ)/1023.0)*5.0;
}

float lectureAmbiant() {
  return (analogRead(pinAmbiant)/1023.0)*5.0;
}

bool detecteSifflet() {
  float valeur5KHZ = lecture5KHZ();
  float valeurAmbiant = lectureAmbiant();

  if (valeur5KHZ > seuilAsbolu && ((valeur5KHZ - valeurAmbiant) > seuilRelatif)) {
        return true;
  } 

  else {
        return false;
  }
}

void setup() {
  BoardInit();
  initParcours(1);
  beep(3);

  pinMode(pin5KHZ, INPUT);
  pinMode(pinAmbiant, INPUT);
  firstRun = true;

  calibrate(5);
  delay(1000);
  beep(2);
  delay(5000);

  //case de départ
  currentTile[0] = 9;
  currentTile[1] = 1;
}

void loop() {
  
  
  if (firstRun){
    beep(1);
    
    while(siffletDetecte == false){
      siffletDetecte = detecteSifflet();
      delay(500);

    }

    firstRun = false;
    beep(2);
  }

  if (currentTile[0] == 0) {
    Serial.println("Run retour");
    initParcours(2);
    depart = true;
    currentTile[0] = 9;
    if (currentTile[1] == 2)
    {
      currentTile[1] = 0;
    } else if (currentTile[1] == 0)
    {
      currentTile[1] = 2;
    }
    
    tourne('L');
    delay(400);
    tourne('L');
    delay(400);
    Serial.print(currentTile[0]);
    Serial.print(", ");
    Serial.print(currentTile[1]);
  }

  if (currentTile[0] == 9 && !depart) {
    Serial.println("Arret");
    arret();
    return;
  }

  //regarde si on peut Avancer si pas de mur et pas de limite
  if (parcours[currentTile[0]][currentTile[1]][0] == 0 && (digitalRead(vertpin) == 1 || digitalRead(rougepin) == 1)) {
    avance(DISTANCE);
    currentTile[0]--;
    depart = false;

    Serial.print(currentTile[0]);
    Serial.print(", ");
    Serial.print(currentTile[1]);
  //sinon vérifie à droite si pas de limite
  } else if (parcours[currentTile[0]][currentTile[1]][1] == 0)
  {
    tourne('R');
    delay(1000);
    arret();

    //si un mur u-turn
    if (digitalRead(vertpin) == 0 || digitalRead(rougepin) == 0)
    {
      //u-turn
      tourne('L');
      delay(400);
      tourne('L');
      delay(400);
      avance(DISTANCE);
      delay(400);
      tourne('R'); //se replace dans la bonne direction
      delay(1000);
    } else {
      //virage à droite normal
      avance(DISTANCE);
      delay(400);
      tourne('L');
      delay(1000);
      currentTile[1]++;
      Serial.print(currentTile[0]);
      Serial.print(", ");
      Serial.print(currentTile[1]);

      //si un mur dans la colonne de droite
      if (currentTile[1] == 2 && (digitalRead(vertpin) == 0 || digitalRead(rougepin) == 0))
      {
        //va à gauche si un mur au milieu et 
        tourne('L');
        delay(400);
        avance(DISTANCE);
        delay(400);
        avance(DISTANCE);
        tourne('R');
        delay(400);
        currentTile[1]--;
        currentTile[1]--;
      }

      Serial.print(currentTile[0]);
      Serial.print(", ");
      Serial.print(currentTile[1]);
    }
  //sinon vérifie à gauche
  } else if (parcours[currentTile[0]][currentTile[1]][3] == 0)
  {
    tourne('L');
    avance(DISTANCE);
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