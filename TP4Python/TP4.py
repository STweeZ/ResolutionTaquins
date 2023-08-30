#!/usr/bin/python3

import sys, os
COUT_UNIFORME = 0
PROFONDEUR_LIMITE = 1
APPROF_ITERATIF = 2
A_ETOILE = 3
IDA_ETOILE = 4
MAX = 1024
SIZE = 3

class Noeud:
    def __init__(self, taquin, poids, heuristique, profondeur, precedent, suivant, suite):
        self.taquin = taquin
        self.poids = poids
        self.heuristique  = heuristique
        self.profondeur = profondeur
        self.precedent = precedent
        self.suivant = suivant
        self.suite = suite

class Liste:
    def __init__(self, noeud):
        self.noeud = noeud

def est_but(taquin):
    for i in range(0,(SIZE * SIZE) - 1):
        if(taquin[i] + 1 != taquin[i+1]):
            return False
    return True

def est_solvable(taquin):
    count=0
    for i in range(0,(SIZE * SIZE) - 1):
        for j in range(i+1,(SIZE*SIZE)):
            if(taquin[i] != 0 and taquin[j] != 0):
                if(taquin[i] > taquin[j]):
                    count += 1
    return count%2 == 0

def print_taquin(taquin, indice_fin):
    indice=(SIZE*SIZE)
    if(indice_fin < indice):
        indice=indice_fin
    for i in range(0,indice):
        print("%d       ",taquin[i])
        if((i+1)%3 == 0):
            print("\n")
    print("\n")

def print_taquin_ligne(taquin, indice_fin):
    indice=(SIZE*SIZE)
    if(indice_fin < indice):
        indice=indice_fin
    for i in range(0,indice):
        print("%d ",taquin[i])

def get_position_item(taquin, item):
    for i in range(0,SIZE*SIZE):
        if(taquin[i] == item):
            return i
    return -1

def deplacementGauche(taquin, position):
    if(position % SIZE != 0):
        taquin[position] = taquin[position - 1]
        taquin[position - 1] = 0
        return 1
    else:
        return 0

def deplacementDroite(taquin, position):
    if(position % SIZE != SIZE - 1):
        taquin[position] = taquin[position + 1]
        taquin[position + 1] = 0
        return 1
    else:
        return 0

def deplacementHaut(taquin, position):
    if(position >= SIZE):
        taquin[position] = taquin[position - SIZE]
        taquin[position - SIZE] = 0
        return 1
    else:
        return 0

def deplacementBas(taquin, position):
    if(position < (SIZE * (SIZE - 1))):
        taquin[position] = taquin[position + SIZE]
        taquin[position + SIZE] = 0
        return 1
    else:
        return 0

def est_solution(taquin, sol):
    size = len(sol)
    position_blanc = 0
    for i in range(0,size):
        position_blanc = get_position_item(taquin, 0)
        if(sol[i].upper == 'G'):
            deplacementGauche(taquin,position_blanc)
        elif(sol[i].upper == 'D'):
            deplacementDroite(taquin,position_blanc)
        elif(sol[i].upper == 'H'):
            deplacementHaut(taquin,position_blanc)
        elif(sol[i].upper == 'B'):
            deplacementBas(taquin,position_blanc)
    return est_but(taquin)

def successeurs(taquin, taquins):
    position_blanc = get_position_item(taquin, 0)
    suite = ""

    taquin_gauche = []
    taquin_droite = []
    taquin_haut = []
    taquin_bas = []

    taquin_gauche = taquin.copy()
    taquin_droite = taquin.copy()
    taquin_haut = taquin.copy()
    taquin_bas = taquin.copy()

    if(deplacementGauche(taquin_gauche,position_blanc)):
        taquins.append(taquin_gauche)
        suite += "G"
    if(deplacementDroite(taquin_droite,position_blanc)):
        taquins.append(taquin_droite)
        suite += "D"
    if(deplacementHaut(taquin_haut,position_blanc)):
        taquins.append(taquin_haut)
        suite += "H"
    if(deplacementBas(taquin_bas,position_blanc)):
        taquins.append(taquin_bas)
        suite += "B"
    return suite

def swap(x, y):
    temp = x
    x = y
    y = temp

def write_taquin(data, taquin, indice_fin):
    indice=(SIZE*SIZE)-1
    if(indice_fin < indice):
        indice=indice_fin
    for i in range(0,indice+1):
        data.write(str(taquin[i]))

def permutation(taquin, debut, fin, data):
    if(debut == fin):
        print_taquin(taquin, fin)
        write_taquin(data, taquin, fin)
        data.write('\n')
    else:
        for i in range(debut, fin + 1):
            swap(taquin[debut], taquin[i])
            permutation(taquin, debut+1, fin, data)
            swap(taquin[debut], taquin[i])

def empiler(pile, taquin, poids, suite, profondeur, heuristique):
    if ((not pile)):
        return False
    noeud = Noeud(taquin,poids,heuristique,profondeur,False,pile.noeud,suite[:])
    if(pile.noeud):
        pile.noeud.precedent = noeud
    pile.noeud = noeud

def depiler(pile):
    if (not pile):
        return False

    noeudDepile = pile.noeud

    if (pile and pile.noeud):
        pile.noeud = noeudDepile.suivant
    return noeudDepile

def retirerUniforme(pile):
    if (not pile):
        return False
    actuel = pile.noeud
    petit=actuel

    while (actuel):
        if(actuel.poids < petit.poids):
            petit = actuel
        actuel = actuel.suivant
    if(petit):
        if(petit==pile.noeud):
            pile.noeud = petit.suivant
            if(pile.noeud):
                pile.noeud.precedent=False
        else:
            petit.precedent.suivant = petit.suivant
        if(petit.suivant):
            petit.suivant.precedent = petit.precedent
    return petit

def retirerEtoile(pile):
    if (not pile):
        return False
    actuel = pile.noeud
    petit=actuel

    while (actuel):
        if((actuel.poids + actuel.heuristique) < (petit.poids + petit.heuristique)):
            petit = actuel
        actuel = actuel.suivant
    if(petit):
        if(petit==pile.noeud):
            pile.noeud = petit.suivant
            if(pile.noeud):
                pile.noeud.precedent = False
        else:
            petit.precedent.suivant = petit.suivant
        if(petit.suivant):
            petit.suivant.precedent = petit.precedent
    return petit

def afficherListe(liste):
    if (not liste):
        return False
    actuel = liste.noeud

    while (actuel):
        print_taquin(actuel.taquin,(SIZE*SIZE))
        actuel = actuel.suivant

def listeVide(liste):
    return not liste.noeud

def taquin_similaire(taquin1, taquin2, size):
    for i in range(0, size):
        if(taquin1[i] != taquin2[i]):
            return 0
    return 1

def a_ete_traite(liste, taquin):
    if (not liste):
        return 0
    actuel = liste.noeud

    while (actuel):
        if(taquin_similaire(actuel.taquin,taquin,(SIZE*SIZE))):
            return 1
        actuel=actuel.suivant
    return 0

def inserer(pile, taquin, poids, suite, strategie, profondeur, heuristique):
    empiler(pile,taquin,poids,suite,profondeur, heuristique)

def retirer(pile, strategie):
    if(strategie == COUT_UNIFORME):
        return retirerUniforme(pile)
    elif((strategie == IDA_ETOILE) or (strategie == A_ETOILE)):
        return retirerEtoile(pile)
    elif(strategie == PROFONDEUR_LIMITE or strategie == APPROF_ITERATIF):
        return depiler(pile)
    return False

def ask_depth():
    depth = input("Jusqu'à quelle profondeur ?\n")
    return depth

def nbCasesF(taquinInit, taquinBut):
    j=0
    for i in range(0, SIZE*SIZE):
        if(taquinInit[i] != taquinBut[i]):
            j+=1
    return j

def manhattan(taquinInit, taquinBut):
    sumPos=0
    for i in range(0,SIZE*SIZE):
        if(taquinInit[i] != 0):
            posInit = get_position_item(taquinInit, taquinInit[i])
            initX = posInit//SIZE
            initY = posInit%SIZE
            posBut = get_position_item(taquinBut, taquinInit[i])
            butX = posBut//SIZE
            butY = posBut%SIZE
            sumPos += abs(butX - initX) + abs(butY - initY)
    return sumPos

def recherche(taquin, taquinBut, strategie, dataWriter):
    profondeur_maximale = False
    profondeur=0
    if(not est_solvable(taquin)):
        dataWriter.write(" -1\n")
        return False
    l = Liste(False)
    l_done = Liste(False)
    inserer(l,taquin,0,"",strategie,0, manhattan(taquin,taquinBut))
    
    if(strategie == PROFONDEUR_LIMITE or strategie == APPROF_ITERATIF):
        profondeur_maximale=ask_depth()
    if(strategie == PROFONDEUR_LIMITE):
        profondeur=profondeur_maximale

    while(not listeVide(l)):
        e = retirer(l,strategie)
        inserer(l_done,e.taquin,e.poids,e.suite,strategie,e.profondeur,e.heuristique)
        if(est_but(e.taquin)):
            dataWriter.write(" " + str(len(e.suite)) + " ")
            dataWriter.write(e.suite)
            dataWriter.write("\n")
            return
        else:
            if((strategie != PROFONDEUR_LIMITE and strategie != APPROF_ITERATIF) or (e.profondeur < profondeur)):
                taquins = []
                suiteSucc = successeurs(e.taquin,taquins)
                for i in range(0, len(suiteSucc)):
                    if((not a_ete_traite(l_done,taquins[i])) and (not a_ete_traite(l,taquins[i]))):
                        suite = e.suite[:]
                        suite += suiteSucc[i]
                        inserer(l,taquins[i],e.poids+1,suite,strategie,e.profondeur+1, manhattan(taquins[i],taquinBut))
        if (listeVide(l) and strategie == APPROF_ITERATIF and profondeur < profondeur_maximale):
            inserer(l,taquin,0,"",strategie,0, manhattan(taquin,taquinBut))
            l_done = Liste(False)
            profondeur+=1

def recherche_idaEtoile(seuil, taquin, taquinBut, dataWriter):
    if(not est_solvable(taquin)):
        dataWriter.write(" -1\n")
        return False
    l = Liste(False)
    l_done = Liste(False)
    inserer(l,taquin,0,"",IDA_ETOILE,0, manhattan(taquin,taquinBut))
    min=4096


    while (not listeVide(l)):
        e = retirer(l,IDA_ETOILE)
        inserer(l_done,e.taquin,e.poids,e.suite,IDA_ETOILE,e.profondeur,e.heuristique)
        if(est_but(e.taquin)):
            dataWriter.write(" " + str(len(e.suite)) + " ")
            dataWriter.write(e.suite)
            dataWriter.write("\n")
            return False
        else:
            taquins = []
            suiteSucc = successeurs(e.taquin,taquins)
            for i in range(0,len(suiteSucc)):
                if((not a_ete_traite(l_done,taquins[i])) and (not a_ete_traite(l,taquins[i]))):
                    suite = e.suite[:]
                    suite += suiteSucc[i]
                    fonctionFFils = manhattan(taquins[i],taquinBut)
                    feprime = e.poids + 1 + fonctionFFils
                    if(feprime <= seuil):
                        inserer(l,taquins[i],e.poids+1,suite,IDA_ETOILE,e.profondeur+1, fonctionFFils)
                    elif(feprime < min):
                        min = feprime
    return recherche_idaEtoile(min, taquin, taquinBut, dataWriter)

def Main():
    if(len(sys.argv) != 2):
        print("Invalid number of parameters")
        return False
    print("Waiting...\n")
    script_dir = os.path.dirname(__file__) # absolute directory the file is in
    rel_path = "../E_O/sortie.txt"
    abs_file_path = os.path.join(script_dir, rel_path)
    fdL = open(sys.argv[1], "r")
    fdW = open(abs_file_path, "w")
    taquinsListen = fdL.read().split("\n")
    taquinBut = list(map(int, list(taquinsListen[0])))
    for i in range(1,len(taquinsListen)):
        if(len(taquinsListen[i]) == 9):
            taquinNormal = list(map(int, list(taquinsListen[i])))
            for t in taquinNormal:
                fdW.write(str(t))
            # recherche(taquinNormal, taquinBut, A_ETOILE, fdW)
            recherche_idaEtoile(manhattan(taquinNormal,taquinBut),taquinNormal, taquinBut, fdW)
    fdW.close()
    fdL.close()

if __name__ == '__main__':
    Main()