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
  parcours[0][1][0] = 1;
  parcours[0][2][0] = 1;

  //bordure droite
  for (int i = 0; i < 10; i++) {
    parcours[i][2][1] = 1;
  }
  
  //bordure du bas
  parcours[9][0][3] = 1;
  parcours[9][1][3] = 1;
  parcours[9][2][3] = 1;

  //bordure gauche
  for (int i = 0; i < 10; i++) {
    parcours[i][0][3] = 1;
  }

  //bordure du milieu lignes : 1,3,5,7
  parcours[1][1][1] = 1;
  parcours[3][1][1] = 1;
  parcours[5][1][1] = 1;
  parcours[7][1][1] = 1;
  parcours[1][1][3] = 1;
  parcours[3][1][3] = 1;
  parcours[5][1][3] = 1;
  parcours[7][1][3] = 1;
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

void setup() {
  BoardInit();
  initParcours();
  //printParcours();

  //case de départ
  currentTile[0] = 9;
  currentTile[1] = 1;
}

void loop() {
  //regarde si on peut Avancer
  if (parcours[currentTile[0]][currentTile[1]][0] == 0) {
    Serial.print("Avance");
  //sinon vérifie à droite
  } else if (parcours[currentTile[0]][currentTile[1]][1] == 0)
  {
    Serial.print("Tourne droite 90");
    Serial.print("Avance");
  //sinon vérifie à gauche
  } else if (parcours[currentTile[0]][currentTile[1]][3] == 0)
  {
    Serial.print("Tourne gauche 90");
    Serial.print("Avance");
  //sinon reviens sur ses pas
  } else {
    Serial.print("Reviens");
  }
  
}