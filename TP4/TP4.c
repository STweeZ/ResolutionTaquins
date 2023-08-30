#include "TP1_2_3.c"
#define COUT_UNIFORME 0
#define PROFONDEUR_LIMITE 1
#define APPROF_ITERATIF 2
#define A_ETOILE 3
#define IDA_ETOILE 4
#define NB_CASES_F 1
#define MANHATTAN 2
#define MANHATTAN_DERIVE 3
#define MAX 200

typedef struct Noeud Noeud;
struct Noeud {
    int *taquin;
    int poids;
    int heuristique;
    int profondeur;
    Noeud *precedent;
    Noeud *suivant;
    char *suite;
};

typedef struct _liste {
    Noeud *noeud;
} Liste;

void empiler(Liste *pile, int *taquin, int poids, char *suite, int profondeur, int heuristique) {
    Noeud *noeud = malloc(sizeof(Noeud));
    if (pile == NULL || noeud == NULL) exit(EXIT_FAILURE);

    noeud->suite = malloc(MAX*sizeof(char));
    strcpy(noeud->suite, suite);
    noeud->taquin = malloc((SIZE * SIZE) * sizeof(int));
    clone_taquin(taquin, noeud->taquin);
    noeud->poids = poids;
    noeud->profondeur = profondeur;
    noeud->heuristique = heuristique;
    noeud->suivant = pile->noeud;
    noeud->precedent = NULL;
    if(pile->noeud) pile->noeud->precedent = noeud;
    pile->noeud = noeud;
    // free(taquin);
}

Noeud *depiler(Liste *pile) {
    if (pile == NULL) exit(EXIT_FAILURE);

    Noeud *noeudDepile = pile->noeud;

    if (pile != NULL && pile->noeud != NULL) pile->noeud = noeudDepile->suivant;
    return noeudDepile;
}

Noeud *retirerUniforme(Liste *pile){
    if (pile == NULL) exit(EXIT_FAILURE);
    Noeud *actuel = pile->noeud;
    Noeud *petit=actuel;

    while (actuel != NULL) {
        if(actuel->poids < petit->poids) petit = actuel;
        actuel = actuel->suivant;
    }
    if(petit != NULL) {
        if(petit==pile->noeud) {
            pile->noeud = petit->suivant;
            if(pile->noeud) pile->noeud->precedent=NULL;
        } else petit->precedent->suivant = petit->suivant;
        if(petit->suivant) {
            petit->suivant->precedent = petit->precedent;
        }
    }
    return petit;
}

Noeud *retirerEtoile(Liste *pile){
    if (pile == NULL) exit(EXIT_FAILURE);
    Noeud *actuel = pile->noeud;
    Noeud *petit=actuel;
    Noeud *new=malloc(sizeof(Noeud));

    while (actuel != NULL) {
        if((actuel->poids + actuel->heuristique) < (petit->poids + petit->heuristique)) petit = actuel;
        actuel = actuel->suivant;
    }
    if(petit != NULL) {
        if(petit==pile->noeud) {
            pile->noeud = petit->suivant;
            if(pile->noeud) pile->noeud->precedent=NULL;
        } else petit->precedent->suivant = petit->suivant;
        if(petit->suivant) {
            petit->suivant->precedent = petit->precedent;
        }
        new->suite = malloc(MAX*sizeof(char));
        strcpy(new->suite, petit->suite);
        new->taquin = malloc((SIZE * SIZE) * sizeof(int));
        clone_taquin(petit->taquin, new->taquin);
        new->poids = petit->poids;
        new->profondeur = petit->profondeur;
        new->heuristique = petit->heuristique;
        new->suivant = NULL;
        new->precedent = NULL;
        free(petit->suite);
        free(petit->taquin);
        free(petit);
    }
    return new;
}

void afficherListe(Liste *liste) {
    if (liste == NULL) exit(EXIT_FAILURE);
    Noeud *actuel = liste->noeud;

    while (actuel != NULL) {
        print_taquin(actuel->taquin,(SIZE*SIZE));
        actuel = actuel->suivant;
    }
}

int listeVide(Liste *liste) {
    return !liste->noeud;
}

int taquin_similaire(int *taquin1, int *taquin2, int size) {
    for(int i=0;i<size;i++) {
        if(taquin1[i] != taquin2[i]) return 0;
    }
    return 1;
}

int a_ete_traite(Liste *liste, int *taquin) {
    if (liste == NULL) return 0;
    Noeud *actuel = liste->noeud;

    while (actuel != NULL) {
        if(taquin_similaire(actuel->taquin,taquin,(SIZE*SIZE))) return 1;
        actuel=actuel->suivant;
    }
    return 0;
}

void inserer(Liste *pile, int *taquin, int poids, char *suite, int strategie, int profondeur, int heuristique) {
    empiler(pile,taquin,poids,suite,profondeur, heuristique);
}

Noeud *retirer(Liste *pile, int strategie) {
    if(strategie == COUT_UNIFORME) return retirerUniforme(pile);
    else if((strategie == IDA_ETOILE) || (strategie == A_ETOILE)) return retirerEtoile(pile);
    else if(strategie == PROFONDEUR_LIMITE || strategie == APPROF_ITERATIF) return depiler(pile);
    return NULL;
}

int ask_depth() {
    int depth;
    printf("Jusqu'à quelle profondeur ?\n");
    scanf("%d",&depth);
    return depth;
}

void viderListe(Liste *liste) {
    if (liste == NULL) return;
    Noeud *actuel=liste->noeud;
    if (actuel == NULL) return;
    while (actuel->suivant != NULL) {
        actuel = actuel->suivant;
        free(actuel->precedent->suite);
        free(actuel->precedent->taquin);
        free(actuel->precedent);
    }
    free(actuel->suite);
    free(actuel->taquin);
    free(actuel);
}

int nbCasesF(int *taquinInit, int *taquinBut) {
    int j=0;
    for(int i=0; i<(SIZE*SIZE); i++) {
        if(taquinInit[i] != taquinBut[i]) j++;
    }
    return j;
}

int manhattan(int *taquinInit, int *taquinBut) {
    int sumPos=0;
    for(int i=0; i<SIZE*SIZE; i++) {
        if(taquinInit[i] != 0) {
            int posInit = get_position_item(taquinInit, taquinInit[i]);
            int initX = posInit/SIZE;
            int initY = posInit%SIZE;
            int posBut = get_position_item(taquinBut, taquinInit[i]);
            int butX = posBut/SIZE;
            int butY = posBut%SIZE;
            sumPos += abs(butX - initX) + abs(butY - initY);
        }
    }
    return sumPos;
}

int deriveManhattan(int *taquinInit, int *taquinBut) {
    int sumPos=0;
    for(int i=0; i<SIZE*SIZE; i++) {
        if(taquinInit[i] != 0) {
            int posInit = get_position_item(taquinInit, taquinInit[i]);
            int initX = posInit/SIZE;
            int initY = posInit%SIZE;
            int posBut = get_position_item(taquinBut, taquinInit[i]);
            int butX = posBut/SIZE;
            int butY = posBut%SIZE;
            sumPos += abs(butX - initX) + abs(butY - initY);
            if(get_position_item(taquinInit, taquinInit[i]) > get_position_item(taquinBut, taquinInit[i])) sumPos +=2;
        }
    }
    return sumPos;
}

void recherche(int *taquin, int *taquinBut, int strategie, dataBufferWriter *dataWriter, int typeHeuristic) {
    int profondeur_maximale;
    int profondeur=0;
    if(!est_solvable(taquin)) {
        write_datas(dataWriter, " -1\n");
        return;
    }
    Liste *l = malloc(sizeof(Liste) * 5000);
    l->noeud = NULL;
    Liste *l_done = malloc(sizeof(Liste) * 5000);
    l_done->noeud = NULL;
    char lasuite[MAX];
    strcpy(lasuite,"");
    if(typeHeuristic == MANHATTAN) inserer(l,taquin,0,lasuite,strategie,0, manhattan(taquin,taquinBut));
    else if(typeHeuristic == MANHATTAN_DERIVE) inserer(l,taquin,0,lasuite,strategie,0, deriveManhattan(taquin,taquinBut));
    else if(typeHeuristic == NB_CASES_F) inserer(l,taquin,0,lasuite,strategie,0, nbCasesF(taquin,taquinBut));
    
    if(strategie == PROFONDEUR_LIMITE || strategie == APPROF_ITERATIF) profondeur_maximale=ask_depth();
    if(strategie == PROFONDEUR_LIMITE) profondeur=profondeur_maximale;

    while(!listeVide(l)) {
        Noeud *e = retirer(l,strategie);
        if(est_but(e->taquin)) {
            char str[5];
            sprintf(str, " %d ", (int)strlen(e->suite));
            write_datas(dataWriter, str);
            write_datas(dataWriter, e->suite);
            write_datas(dataWriter, "\n");
            free(e->suite);
            free(e->taquin);
            free(e);
            viderListe(l);
            viderListe(l_done);
            free(l);
            free(l_done);
            return;
        }
        inserer(l_done,e->taquin,e->poids,e->suite,strategie,e->profondeur,e->heuristique);
        if((strategie != PROFONDEUR_LIMITE && strategie != APPROF_ITERATIF) || (e->profondeur < profondeur)) {
            char suiteSucc[5];
            int **taquins = malloc(4 * sizeof(int *));
            strcpy(suiteSucc,"");
            int row = successeurs(e->taquin,suiteSucc,taquins);
            for(int i=0; i<row; i++) {
                if(!a_ete_traite(l_done,taquins[i]) && !a_ete_traite(l,taquins[i])) {
                    char suite[MAX];
                    strcpy(suite,e->suite);
                    strncat(suite,&suiteSucc[i],1);
                    if(typeHeuristic == MANHATTAN) inserer(l,taquins[i],e->poids+1,suite,strategie,e->profondeur+1, manhattan(taquins[i],taquinBut));
                    else if(typeHeuristic == MANHATTAN_DERIVE) inserer(l,taquins[i],e->poids+1,suite,strategie,e->profondeur+1, deriveManhattan(taquins[i],taquinBut));
                    else if(typeHeuristic == NB_CASES_F) inserer(l,taquins[i],e->poids+1,suite,strategie,e->profondeur+1, nbCasesF(taquins[i],taquinBut));
                }
                free(taquins[i]);
            }
            free(taquins);
            free(e->suite);
            free(e->taquin);
            free(e);
        }
        if (listeVide(l) && strategie == APPROF_ITERATIF && profondeur < profondeur_maximale) {
            char suite[MAX];
            strcpy(suite,"");
            if(typeHeuristic == MANHATTAN) inserer(l,taquin,0,suite,strategie,0, manhattan(taquin,taquinBut));
            else if(typeHeuristic == MANHATTAN_DERIVE) inserer(l,taquin,0,suite,strategie,0, deriveManhattan(taquin,taquinBut));
            else if(typeHeuristic == NB_CASES_F) inserer(l,taquin,0,suite,strategie,0, nbCasesF(taquin,taquinBut));
            free(l_done);
            l_done = malloc(sizeof(Liste) * 5000);
            l_done->noeud = NULL;
            profondeur++;
        }
    }
    viderListe(l);
    viderListe(l_done);
    free(l);
    free(l_done);
}

void recherche_idaEtoile(int seuil, int *taquin, int *taquinBut, dataBufferWriter *dataWriter, int typeHeuristic) {
    if(!est_solvable(taquin)) {
        write_datas(dataWriter, " -1\n");
        return;
    }
    Liste *l = malloc(sizeof(Liste) * 5000);
    l->noeud = NULL;
    Liste *l_done = malloc(sizeof(Liste) * 5000);
    l_done->noeud = NULL;
    char lasuite[MAX];
    strcpy(lasuite,"");
    if(typeHeuristic == MANHATTAN) inserer(l,taquin,0,lasuite,IDA_ETOILE,0, manhattan(taquin,taquinBut));
    else if(typeHeuristic == MANHATTAN_DERIVE) inserer(l,taquin,0,lasuite,IDA_ETOILE,0, deriveManhattan(taquin,taquinBut));
    else if(typeHeuristic == NB_CASES_F) inserer(l,taquin,0,lasuite,IDA_ETOILE,0, nbCasesF(taquin,taquinBut));
    int min=4096;

    while (!listeVide(l)) {
        Noeud *e = retirer(l,IDA_ETOILE);
        if(est_but(e->taquin)) {
            char str[5];
            sprintf(str, " %d ", (int)strlen(e->suite));
            write_datas(dataWriter, str);
            write_datas(dataWriter, e->suite);
            write_datas(dataWriter, "\n");
            free(e->suite);
            free(e->taquin);
            free(e);
            viderListe(l);
            viderListe(l_done);
            free(l);
            free(l_done);
            return;
        }
        inserer(l_done,e->taquin,e->poids,e->suite,IDA_ETOILE,e->profondeur,e->heuristique);
        char suiteSucc[5];
        int **taquins = malloc(4 * sizeof(int *));
        strcpy(suiteSucc,"");
        int row = successeurs(e->taquin,suiteSucc,taquins);
        for(int i=0; i<row; i++) {
            if(!a_ete_traite(l_done,taquins[i]) && !a_ete_traite(l,taquins[i])) {
                char suite[MAX];
                strcpy(suite,e->suite);
                strncat(suite,&suiteSucc[i],1);
                int fonctionFFils = 0;
                if(typeHeuristic == MANHATTAN) fonctionFFils = manhattan(taquins[i],taquinBut);
                else if(typeHeuristic == MANHATTAN_DERIVE) fonctionFFils = deriveManhattan(taquins[i],taquinBut);
                else if(typeHeuristic == NB_CASES_F) fonctionFFils = nbCasesF(taquins[i],taquinBut);
                int feprime = e->poids + 1 + fonctionFFils;
                if(feprime <= seuil) inserer(l,taquins[i],e->poids+1,suite,IDA_ETOILE,e->profondeur+1, fonctionFFils);
                else if(feprime < min) min = feprime;
            }
            free(taquins[i]);
        }
        free(taquins);
        free(e->suite);
        free(e->taquin);
        free(e);
    }
    viderListe(l);
    viderListe(l_done);
    free(l);
    free(l_done);
    return recherche_idaEtoile(min, taquin, taquinBut, dataWriter, typeHeuristic);
}

int main(int argc, char *argv[]) {
    char in[MAX] = "";
    printf("Input file ?\n");
    scanf("%s",in);
    char out[MAX] = "";
    printf("Output file ?\n");
    scanf("%s",out);
    dataBufferReader *dataReader = createBufferReader(in);
    dataBufferWriter *dataWriter = createBufferWriter(out);
    int typeAnalysis = -1;
    while(typeAnalysis != 1 && typeAnalysis != 2) {
        printf("Analysis Type ?\nType 1 for A*\t-- or --\tType 2 for IDA*\n");
        scanf("%d",&typeAnalysis);
    }
    int typeHeuristic = -1;
    while(typeHeuristic != 1 && typeHeuristic != 2 && typeHeuristic != 3) {
        printf("Heuristic Type ?\nType 1 for nbCasesF\t-- or --\tType 2 for Manhattan\t-- or --\tType 3 for Manhattan drift\n");
        scanf("%d",&typeHeuristic);
    }
    printf("Waiting...\n");
    int *sBut = malloc((SIZE * SIZE) * sizeof(int));
    int isSet = 0;
    char s[SIZE*SIZE];
    strcpy(s,"");
    char c;
    while((c = getCurrentChar(dataReader)) != -1) {
        if(c == '\n') {
            if(!isSet) {
                isSet = 1;
                for(int i=0; i<(SIZE*SIZE); i++) sBut[i] = s[i] - '0';
            } else {
                int *taquin = malloc((SIZE * SIZE) * sizeof(int));
                for(int i=0; i<(SIZE*SIZE); i++) taquin[i] = s[i] - '0';
                write_taquin(dataWriter, taquin, SIZE*SIZE);
                if(typeAnalysis == 1 && typeHeuristic == 1) recherche(taquin, sBut, A_ETOILE, dataWriter, NB_CASES_F);
                else if(typeAnalysis == 1 && typeHeuristic == 2) recherche(taquin, sBut, A_ETOILE, dataWriter, MANHATTAN);
                else if(typeAnalysis == 1 && typeHeuristic == 3) recherche(taquin, sBut, A_ETOILE, dataWriter, MANHATTAN_DERIVE);
                else if(typeAnalysis == 2 && typeHeuristic == 1) recherche_idaEtoile(nbCasesF(taquin,sBut),taquin, sBut, dataWriter, NB_CASES_F);
                else if(typeAnalysis == 2 && typeHeuristic == 2) recherche_idaEtoile(manhattan(taquin,sBut),taquin, sBut, dataWriter, MANHATTAN);
                else if(typeAnalysis == 2 && typeHeuristic == 3) recherche_idaEtoile(manhattan(taquin,sBut),taquin, sBut, dataWriter, MANHATTAN_DERIVE);
                free(taquin);
            }
            memset(s,0,SIZE*SIZE);
        } else strncat(s,&c,1);
        consumeChar(dataReader);
    }
    free(sBut);
    destroyBufferWriter(dataWriter);
    destroyBufferReader(dataReader);
}