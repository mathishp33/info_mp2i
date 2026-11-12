#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>


void affiche(int tab[], int n) {
    for (int i = 0; i < n; i += 1) {
        printf("%d-", tab[i]);
    }
    printf("\n");
}

/* Partie 1 */

int rechSeqLast(int tab[], int n, int x){
    for (int i = 0; i < n; i += 1){
        if (tab[i] == x) {
            while (tab[i] == x) { i++; }
            return i - 1;
        }
    }
    return -1;
}

int rechSeqFirst(int tab[], int n, int x){
    for (int i = 0; i < n; i += 1)
        if (tab[i] == x)
            return i;

    return -1;
}

int rechDicho(int tab[], int n, int x){
    int a = 0; // début
    int b = n; // fin
    while (b - a > 0) {
        // invariant : b - a > 0
        //
        int m = (a + b) / 2; // NB : division entière
        if (tab[m] == x) { // valeur trouvée
            return m;
        } else if (tab[m] < x) { // cherche à droite
            a = m + 1;
        } else { // cherche à gauche
            b = m;
        }
    }
    // valeur pas trouvée
    return -1;
}


void testRechDicho() {
    int T[9] = {-20, -12, 4, 5, 5, 7, 18, 18, 21};
    for (int x = -100; x <= 100; x += 1){
        int i1 = rechSeqLast(T, 9, x);
        int i2 = rechDicho(T, 9, x);
        assert(i1 == i2);
    }   
}


/* Partie 2 */

int rechDichoRecAux(int tab[], int a, int b, int x) {
    int m = (a + b) / 2;
    if (!(b - a))
        return -1;
    if (tab[m] == x)
        return m;
    if (tab[m] < x)
        return rechDichoRecAux(tab, m + 1, b, x);
    return rechDichoRecAux(tab, a, m, x);
}

int rechDichoRec(int tab[], int n, int x){
    return rechDichoRecAux(tab, 0, n, x);
}


void testRechDichoRec() {
    int T[9] = {-20, -12, 4, 5, 5, 7, 18, 18, 21};
    for (int x = -100; x <= 100; x += 1){
        int i1 = rechSeqLast(T, 9, x);
        int i2 = rechDichoRec(T, 9, x);
        assert(i1 == i2);
    }   
}


/* Partie 3 */

int rechPremDicho(int tab[], int n, int x){
    int a = 0; // début de la zone de recherche (inclus)
    int b = n; // fin de la zone de recherche (exclu)
    int pos = -1; // dernière position de x
    while (b - a > 0) {
        // invariant : b - a > 0
        //
        int m = (a + b) / 2; // NB : division entière
        if (tab[m] == x) { // valeur trouvée
            pos = m;
            b = m;
        } else if (tab[m] < x) { // cherche à droite
            a = m + 1;
        } else { // cherche à gauche
            b = m;
        }
    }
    return pos;
}

void testRechPremDicho() {
    int T[9] = {-20, -12, 4, 5, 5, 7, 18, 18, 21};
    for (int x = -100; x <= 100; x += 1){
        int i1 = rechSeqFirst(T, 9, x);
        int i2 = rechPremDicho(T, 9, x);
        assert(i1 == i2); 
    }
}

/* Partie 4 */

bool rechTab2DDicho(int T[7][6], int n, int p, int x) {
    for (int i = 0; i < n; i++) {
        int a = 0; // début
        int b = p; // fin
        while (b - a > 0) {
            // invariant : b - a > 0
            //
            int m = (a + b) / 2; // NB : division entière
            if (T[i][m] == x) { // valeur trouvée
                return true;
            } else if (T[i][m] < x) { // cherche à droite
                a = m + 1;
            } else { // cherche à gauche
                b = m;
            }
        }
    }
    return false;
}

int rechSeq2D(int tab[7][6], int n, int p, int x) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < p; j++)
            if (tab[i][j] == x)
                return true;
    return false;
}

void testRechTab2DDicho() {
    int T[7][6] = {
        { -12, -1,  5, 20, 40, 40},
        { -12,  0,  7, 21, 40, 45},
        { -10,  0, 11, 22, 40, 45},
        {  -3,  0, 13, 26, 40, 55},
        {  -3,  2, 13, 30, 40, 56},
        {  -1,  2, 14, 40, 40, 79},
        {  -1,  3, 15, 40, 40, 99}
    };

    for (int x = -100; x <= 100; x += 1){
        int i1 = rechSeq2D(T, 7, 6, x);
        int i2 = rechTab2DDicho(T, 7, 6, x);
        printf("x = %d; i1 = %d; i2 = %d \n", x, i1, i2);
        assert(i1 == i2);
    }   
}


/* Programme principal */

int main(){
    printf("TP recherche par dichotomie dÃ©but\n");

    testRechDicho(); printf("testRechDicho OK\n");
    testRechDichoRec(); printf("testRechDichoRec OK\n");
    testRechPremDicho(); printf("testRechPremDicho OK\n");
    testRechTab2DDicho(); printf("testRechTab2DDicho OK\n");

    printf("TP recherche par dichotomie fin\n");
    return 0;
};

