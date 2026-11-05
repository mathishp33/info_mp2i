#include <stdio.h>

struct vecteur_s {
    double *v;
    int taille;
    int num_classe;
};
typedef struct vecteur_s vecteur;

struct noeud_s {
    vecteur *c;
    struct noeud_s *suivant;
};
typedef struct noeud_s noeud;

void ajoutVecteur(vecteur *vec, noeud **tete) {
    noeud nouvelle_tete;
    nouvelle_tete.c = vec;
    tete->suivant = &nouvelle_tete;
}

int main() {



    return 0;
}