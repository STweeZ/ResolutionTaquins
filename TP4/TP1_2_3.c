#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../annexes/reader.c"
#define SIZE 3

bool est_but(int *taquin) { // EXO 1
    for(int i=0; i<(SIZE*SIZE) - 1; i++) {
        if(taquin[i] + 1 != taquin[i+1]) return false;
    }
    return true;
}

bool est_solvable(int *taquin) { // EXO 2
    int count=0;
    for(int i=0; i<(SIZE*SIZE) - 1; i++) {
        for(int j=i+1; j<(SIZE*SIZE); j++) {
            if(taquin[i] != 0 && taquin[j] != 0) {
                if(taquin[i] > taquin[j]) count++;
            }
        }
    }
    return count%2 == 0;
}

void print_taquin(int *taquin, int indice_fin) {
    int indice=(SIZE*SIZE);
    if(indice_fin < indice) indice=indice_fin;
    for(int i=0; i<indice; i++) {
        printf("%d       ",taquin[i]);
        if((i+1)%3 == 0) printf("\n");
    }
    printf("\n");
}

void print_taquin_ligne(int *taquin, int indice_fin) {
    int indice=(SIZE*SIZE);
    if(indice_fin < indice) indice=indice_fin;
    for(int i=0; i<indice; i++) {
        printf("%d ",taquin[i]);
    }
}

int get_position_item(int *taquin, int item) {
    for(int i=0; i<(SIZE*SIZE); i++) {
        if(taquin[i] == item) return i;
    }
    return -1;
}

bool deplacementGauche(int *taquin, int position) {
    if(position % SIZE != 0) {
        taquin[position] = taquin[position - 1];
        taquin[position - 1] = 0;
        return 1;
    } else return 0;
}

bool deplacementDroite(int *taquin, int position) {
    if(position % SIZE != SIZE - 1) {
        taquin[position] = taquin[position + 1];
        taquin[position + 1] = 0;
        return 1;
    } else return 0;
}

bool deplacementHaut(int *taquin, int position) {
    if(position >= SIZE) {
        taquin[position] = taquin[position - SIZE];
        taquin[position - SIZE] = 0;
        return 1;
    } else return 0;
}

bool deplacementBas(int *taquin, int position) {
    if(position < (SIZE * (SIZE - 1))) {
        taquin[position] = taquin[position + SIZE];
        taquin[position + SIZE] = 0;
        return 1;
    } else return 0;
}

bool est_solution(int *taquin, char *sol) { // EXO 3
    int size = strlen(sol);
    int position_blanc;
    for(int i=0; i<size; i++) {
        position_blanc = get_position_item(taquin, 0);
        switch(toupper(sol[i])) {
            case 'G':
                deplacementGauche(taquin,position_blanc);
                break;
            case 'D':
                deplacementDroite(taquin,position_blanc);
                break;
            case 'H':
                deplacementHaut(taquin,position_blanc);
                break;
            case 'B':
                deplacementBas(taquin,position_blanc);
                break;
        }
    }
    return est_but(taquin);
}

void clone_taquin(int *taquin, int *taquin_clone) {
    for(int i=0; i<(SIZE*SIZE); i++) {
        taquin_clone[i] = *taquin++;
    }
}

int successeurs(int *taquin, char *suite, int **taquins) { // EXO 4
    int position_blanc = get_position_item(taquin, 0);
    int cpt = 0;

    int *taquin_gauche = malloc((SIZE * SIZE) * sizeof(int));
    int *taquin_droite = malloc((SIZE * SIZE) * sizeof(int));
    int *taquin_haut = malloc((SIZE * SIZE) * sizeof(int));
    int *taquin_bas = malloc((SIZE * SIZE) * sizeof(int));

    clone_taquin(taquin, taquin_gauche);
    clone_taquin(taquin, taquin_droite);
    clone_taquin(taquin, taquin_haut);
    clone_taquin(taquin, taquin_bas);

    if(deplacementGauche(taquin_gauche,position_blanc)) {
        taquins[cpt++] = taquin_gauche;
        strcat(suite,"G");
    } else free(taquin_gauche);
    if(deplacementDroite(taquin_droite,position_blanc)) {
        taquins[cpt++] = taquin_droite;
        strcat(suite,"D");
    } else free(taquin_droite);
    if(deplacementHaut(taquin_haut,position_blanc)) {
        taquins[cpt++] = taquin_haut;
        strcat(suite,"H");
    } else free(taquin_haut);
    if(deplacementBas(taquin_bas,position_blanc)) {
        taquins[cpt++] = taquin_bas;
        strcat(suite,"B");
    } else free(taquin_bas);
    strcat(suite,"\0");
    return cpt;
}

void swap(int *x, int *y) {
    int temp;
    temp = *x;
    *x = *y;
    *y = temp;
}

void write_taquin(dataBufferWriter *data, int *taquin, int indice_fin) {
    int indice=(SIZE*SIZE)-1;
    if(indice_fin < indice) indice=indice_fin;
    for(int i=0; i<=indice; i++) {
        putUnsignedChar(data, taquin[i]+'0');
    }
}

void write_datas(dataBufferWriter *data, char *text) {
    for(int i=0; i < strlen(text); i++) {
        putUnsignedChar(data, text[i]);
    }
}

void permutation(int *taquin, int debut, int fin, dataBufferWriter *data) {
    int i;
    if(debut == fin) {
        print_taquin(taquin, fin);
        write_taquin(data, taquin, fin);
        putUnsignedChar(data, '\n');
    } else {
        for(i = debut; i <= fin; i++) {
            swap(&taquin[debut], &taquin[i]);
            permutation(taquin, debut+1, fin, data);
            swap(&taquin[debut], &taquin[i]);
        }
    }
}