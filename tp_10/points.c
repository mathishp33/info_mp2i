/* points.c */
    
#include <stdio.h>


typedef struct point2D_ {
    float x;
    float y;

} point2D;

void affichage(point2D *p) {
    printf("P(x = %f, y = %f) \n", p->x, p->y);
}

void echangeCoord(point2D *p) {
    float temp = p->x;
    p->x = p->y;
    p->y = temp;
}

point2D pointUtilisateur() {
    point2D p = { .x = 0.0, .y = 0.0 };
    printf("choisir nouveau point: ");
    scanf("(%f, %f)", &p.x, &p.y);
    printf("\n");
    return p;
}

void coins(point2D centre, float c, point2D *pta, point2D *ptb, point2D *ptc, point2D *ptd) {
    float d = c / 2;
    pta->x = centre.x - d; pta->y = centre.y + d;
    ptb->x = centre.x + d; ptb->y = centre.y + d;
    ptc->x = centre.x - d; ptc->y = centre.y - d;
    ptd->x = centre.x + d; ptd->y = centre.y - d;
}

int main() {
    point2D p = { .x = 2.5, .y = 5.5 };
    affichage(&p);
    echangeCoord(&p);
    affichage(&p);
    
    point2D p2 = pointUtilisateur();
    affichage(&p2);
    point2D pta; point2D ptb; point2D ptc; point2D ptd;
    float c = 1.0;
    coins(p2, c, &pta, &ptb, &ptc, &ptd);
    affichage(&pta);
    affichage(&ptb);
    affichage(&ptc);
    affichage(&ptd);

    return 0;
}

