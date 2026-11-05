#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>


void affiche(int tab[], int n) {
    printf("t = { ");
    for (size_t i = 0; i < n; i++) {
        printf("%d", tab[i]);
        if (i != n - 1)
            printf(", ");
    }
    printf(" } \n");
}


/* Partie 1 */

void swap(int *i, int *j) {
    int temp = *j;
    *j = *i;
    *i = temp;
}

void triSelection(int tab[], size_t n) {
    for (size_t i = 0; i < n - 1; i++)
        for (size_t j = i + 1; j < n; j++)
            if (tab[i] > tab[j])
                swap(&tab[i], &tab[j]);
}

bool egaux(int t1[], int t2[], int n) {
    for (size_t i = 0; i < n; i++)
        if (t1[i] != t2[i])
            return false;
    return true;
}

void testTriSelection() {
    int T[7] = {3,6,3,8,5,-1,4};
    int TT[7] = {-1,3,3,4,5,6,8};
    triSelection(T, 7);
    assert(egaux(T, TT, 7));
}

/* Partie 2 */
void triSelectionDecr(int tab[], int n) {
    for (int i = n - 1; i > 0 ; i--)
        for (int j = i - 1; j >= 0; j--)
            if (tab[i] > tab[j])
                swap(&tab[i], &tab[j]); 
}

void testTriSelectionDecr() {
    int T1[7] = {3,6,3,8,5,-1,4};
    int TT1[7] = {8,6,5,4,3,3,-1};
    triSelectionDecr(T1, 7);
    assert(egaux(T1, TT1, 7));
}


/* Partie 3 */

int abs(int x) {
    return x < 0 ? -x : x;
}

int comp(int x, int y){
    return abs(x) > abs(y);
}

void triSelectionComp(int tab[], int n){
    for (size_t i = 0; i < n - 1; i++)
        for (size_t j = i + 1; j < n; j++)
            if (comp(tab[i], tab[j]))
                swap(&tab[i], &tab[j]);
}

void testTriSelectionVA() {
    int T2[7] = {-11,-6,3,8,5,-1,4};
    int TT2[7] = {-1,3,4,5,-6,8,-11};
    triSelectionComp(T2, 7);
    assert(egaux(T2, TT2, 7));
}


/* Partie 4 */

typedef struct point2D {
    int x; 
    int y;
} point2D;

void swap2D(point2D *i, point2D *j) {
    point2D temp = *j;
    *j = *i;
    *i = temp;
}

void triParAbsCr(point2D tab[], int n){
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (tab[i].x > tab[j].x)
                swap2D(&tab[i], &tab[j]);
}

void afficheP2D(point2D tab[], int n) {
    for (int i = 0; i < n; i += 1) {
        printf("(%d,%d)-", tab[i].x, tab[i].y);
    }
    printf("\n");
}

bool egauxP2D(point2D t1[], point2D t2[], int n){
    for (size_t i = 0; i < n; i++)
        if (t1[i].x != t2[i].x || t1[i].y != t2[i].y)
            return false;
    return true;
}

void testTriParAbsCr() {
    point2D T3[7] = {{.x=3,.y=1},{.x=6,.y=0},{.x=3,.y=7},{.x=8,.y=4},{.x=5,.y=0},{.x=-1,.y=4},{.x=4,.y=9}};
    point2D TT3[7] = {{.x=-1,.y=4},{.x=3,.y=7},{.x=3,.y=1},{.x=4,.y=9},{.x=5,.y=0},{.x=6,.y=0},{.x=8,.y=4}};
    triParAbsCr(T3, 7);
    assert(egauxP2D(T3, TT3, 7));
}

void TriParInsertion(int tab[], int n) {
    for (int i = 1; i < n; i++) {
        int x = tab[i];
        int j = i;
        while (j > 0 && tab[j - 1] > x) {
            tab[j] = tab[j - 1];
            j--;
        }
        tab[j] = x;
   }
}

void testTriInsertion() {
    int T[7] = {3,6,3,8,5,-1,4};
    int TT[7] = {-1,3,3,4,5,6,8};
    TriParInsertion(T, 7);
    assert(egaux(T, TT, 7));
}

void TriParComptage(int *T, int n, int max) {
    int *comptage = malloc(max * 4); //int comptage[21] = { 0 };
    for (size_t i = 0; i < n; i++)
        comptage[T[i]]++;
    size_t idx = 0;
    for (size_t i = 0; i < max; i++) {
        for (size_t j = 0; j < comptage[i]; j++) {
            T[idx] = i;
            idx++;
        }
    }
}

void testTriComptage() {
    int T[7] = {3,6,3,8,5,1,4};
    int TT[7] = {1,3,3,4,5,6,8};
    TriParComptage(T, 7, 21);
    assert(egaux(T, TT, 7));
}

int main() {
    testTriSelection();
    testTriSelectionDecr();
    testTriSelectionVA();
    testTriParAbsCr();
    testTriInsertion();
    testTriComptage();

    return 0;
}
