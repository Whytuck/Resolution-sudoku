#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define N 3
#define TAILLE (N * N)

typedef struct {
  int valeur;
  bool candidats[TAILLE + 1];
  int nbCandidats;
} tCase2;

typedef tCase2 tGrille[TAILLE][TAILLE];

char nomFichier[30];

int chargerGrille(tGrille grille);
void ajouterCandidat(tCase2 *cellule, int valeur);
void retirerCandidat(tCase2 *cellule, int valeur);
bool estCandidat(tCase2 cellule, int valeur);
int nbCandidatsCase(tCase2 cellule);
void afficherStats(tGrille grille);
bool possible(tGrille grille, int ligne, int colonne, int valeur);
void initCandidats(tGrille grille, int nbCasesVides);
void singletonNu(tGrille grille, int colonne, int ligne, bool *progression);
void singletonCache(tGrille grille, int colonne, int ligne, bool *progression);
void pairesNues(tGrille grille, bool *progression);
void pairesCachees(tGrille grille, bool *progression);

int main() {
  tGrille grille;
  bool progression;
  int nbCasesVides;
  printf("Nom du fichier : ");
  scanf("%s", nomFichier);
  nbCasesVides = chargerGrille(grille);
  initCandidats(grille, nbCasesVides);
  progression = true;
  while (nbCasesVides != 0 && progression) {
    progression = false;
    for (int col = 0; col < TAILLE; col++) {
      for (int lig = 0; lig < TAILLE; lig++) {
        if (grille[lig][col].valeur == 0) {
          singletonNu(grille, col, lig, &progression);
          singletonCache(grille, col, lig, &progression);
        }
      }
    }
    pairesNues(grille, &progression);
    pairesCachees(grille, &progression);
  }
  afficherStats(grille);
}

int chargerGrille(tGrille grille) {
  int count = 0;
  FILE *f;
  f = fopen(nomFichier, "rb");
  if (f == NULL) {
    printf("\n ERREUR sur le fichier %s\n", nomFichier);
    exit(EXIT_FAILURE);
  } else {
    for (int i = 0; i < TAILLE; i++) {
      for (int j = 0; j < TAILLE; j++) {
        fread(&grille[i][j].valeur, sizeof(int), 1, f);
        grille[i][j].nbCandidats = 0;
        for (int k = 0; k <= TAILLE; k++) {
          grille[i][j].candidats[k] = false;
        }
        if (grille[i][j].valeur == 0) {
          count++;
        }
      }
    }
    fclose(f);
    return count;
  }
}

void ajouterCandidat(tCase2 *cellule, int valeur) {
  cellule->nbCandidats++;
  cellule->candidats[valeur] = true;
}

void retirerCandidat(tCase2 *cellule, int valeur) {
  if (cellule->candidats[valeur]) {
    cellule->nbCandidats--;
    cellule->candidats[valeur] = false;
  }
}

bool estCandidat(tCase2 cellule, int valeur) {
  return cellule.candidats[valeur];
}

int nbCandidatsCase(tCase2 cellule) { return cellule.nbCandidats; }

void afficherStats(tGrille grille) {
  int nbCasesRemplies = 0;
  int nbCandidatsElimines = 0;
  float tauxRemplissage;
  float tauxElimination;

  for (int i = 0; i < TAILLE; i++) {
    for (int j = 0; j < TAILLE; j++) {
      if (grille[i][j].valeur != 0) {
        nbCasesRemplies++;
      }
      nbCandidatsElimines += TAILLE - nbCandidatsCase(grille[i][j]);
    }
  }
  tauxRemplissage = (float)nbCasesRemplies / (TAILLE * TAILLE);
  tauxElimination = (float)nbCandidatsElimines / (TAILLE * TAILLE * TAILLE);

  printf("\n****** RESULTATS POUR %s ******\n", nomFichier);
  printf("Nombre de cases remplies = %d sur %d           ", nbCasesRemplies,
         TAILLE * TAILLE);
  printf("Taux de remplissage = %.2f %%\n", tauxRemplissage * 100);
  printf("Nombre de candidats éliminés = %d sur %d     ", nbCandidatsElimines,
         TAILLE * TAILLE * TAILLE);
  printf("Taux d'élimination = %.2f %%\n", tauxElimination * 100);
}

bool possible(tGrille grille, int ligne, int colonne, int valeur) {
  bool possible = true;
  for (int i = 0; i < TAILLE; i++) {
    if (grille[ligne][i].valeur == valeur) {
      possible = false;
    }
  }
  for (int i = 0; i < TAILLE; i++) {
    if (grille[i][colonne].valeur == valeur) {
      possible = false;
    }
  }
  int ligneCarre = ligne / N;
  int colonneCarre = colonne / N;
  for (int i = 0; i < N; i++) {
    for (int l = 0; l < N; l++) {
      if (grille[ligneCarre * N + i][colonneCarre * N + l].valeur == valeur) {
        possible = false;
      }
    }
  }
  return possible;
}

void initCandidats(tGrille grille, int nbCasesVides) {
  for (int lig = 0; lig < TAILLE; lig++) {
    for (int col = 0; col < TAILLE; col++) {
      if (grille[lig][col].valeur == 0) {
        for (int val = 1; val <= TAILLE; val++) {
          if (possible(grille, lig, col, val)) {
            ajouterCandidat(&grille[lig][col], val);
          }
        }
      }
    }
  }
}

void singletonNu(tGrille grille, int col, int lig, bool *progression) {
  if (nbCandidatsCase(grille[lig][col]) == 1) {
    *progression = true;
    bool trouve = false;
    int count = 1;
    while (count <= TAILLE && !trouve) {
      if (grille[lig][col].candidats[count]) {
        trouve = true;
      } else {
        count++;
      }
    }
    grille[lig][col].valeur = count;
    for (int i = 0; i < TAILLE; i++) {
      retirerCandidat(&grille[lig][i], count);
      retirerCandidat(&grille[i][col], count);
    }
    int ligneCarre = lig / N;
    int colonneCarre = col / N;
    for (int i = 0; i < N; i++) {
      for (int l = 0; l < N; l++) {
        retirerCandidat(&grille[ligneCarre * N + i][colonneCarre * N + l],
                        count);
      }
    }
  }
}

void singletonCache(tGrille grille, int col, int lig, bool *progression) {
  for (int i = 0; i < TAILLE; i += N) {
    for (int j = 0; j < TAILLE; j += N) {
      for (int val = 1; val <= TAILLE; val++) {
        int count = 0;
        int ligCase;
        int colCase;
        for (int l = 0; l < N; l++) {
          for (int c = 0; c < N; c++) {
            ligCase = i + l;
            colCase = j + c;
            if (grille[ligCase][colCase].valeur == 0 &&
                estCandidat(grille[ligCase][colCase], val)) {
              count++;
            }
          }
        }
        if (count == 1) {
          *progression = true;
          for (int l = 0; l < N; l++) {
            for (int c = 0; c < N; c++) {
              ligCase = i + l;
              colCase = j + c;
              if (grille[ligCase][colCase].valeur == 0 &&
                  estCandidat(grille[ligCase][colCase], val)) {
                grille[ligCase][colCase].valeur = val;
                for (int a = 0; a < TAILLE; a++) {
                  retirerCandidat(&grille[ligCase][a], val);
                  retirerCandidat(&grille[a][colCase], val);
                }
                int ligneCarre = ligCase / N;
                int colonneCarre = colCase / N;
                for (int a = 0; a < N; a++) {
                  for (int b = 0; b < N; b++) {
                    retirerCandidat(
                        &grille[ligneCarre * N + a][colonneCarre * N + b], val);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

void pairesNues(tGrille grille, bool *progression) {
  for (int bloc = 0; bloc < TAILLE; bloc++) {
    for (int val = 1; val <= TAILLE; val++) {
      int count = 0;
      int lig1, col1, lig2, col2;
      for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
          if (grille[i][j].valeur == 0 && estCandidat(grille[i][j], val)) {
            if (count == 0) {
              lig1 = i;
              col1 = j;
              count++;
            } else if (count == 1 && i != lig1 && j != col1) {
              lig2 = i;
              col2 = j;
              count++;
            }
          }
        }
      }
      if (count == 2) {
        for (int i = 0; i < TAILLE; i++) {
          for (int j = 0; j < TAILLE; j++) {
            if ((i < bloc * N || i >= (bloc + 1) * N) ||
                (j < bloc * N || j >= (bloc + 1) * N)) {
              if ((i != lig1 || j != col1) && (i != lig2 || j != col2)) {
                retirerCandidat(&grille[i][j], val);
                *progression = true;
              }
            }
          }
        }
      }
    }
  }
}

void pairesCachees(tGrille grille, bool *progression) {
  for (int bloc = 0; bloc < TAILLE; bloc++) {
    for (int val = 1; val <= TAILLE; val++) {
      int count = 0;
      int lig1, col1, lig2, col2;
      for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
          if (grille[i][j].valeur == 0 && estCandidat(grille[i][j], val)) {
            if (count == 0) {
              lig1 = i;
              col1 = j;
              count++;
            } else if (count == 1 && i != lig1 && j != col1) {
              lig2 = i;
              col2 = j;
              count++;
            }
          }
        }
      }
      if (count == 2) {
        for (int i = bloc * N; i < (bloc + 1) * N; i++) {
          for (int j = bloc * N; j < (bloc + 1) * N; j++) {
            if (i != lig1 || j != col1) {
              retirerCandidat(&grille[i][j], val);
              *progression = true;
            }
            if (i != lig2 || j != col2) {
              retirerCandidat(&grille[i][j], val);
              *progression = true;
            }
          }
        }
      }
    }
  }
}
