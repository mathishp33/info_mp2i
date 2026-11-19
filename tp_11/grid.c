#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

typedef struct point2D {
    float x, y;
  } Point2d;
   
float randf(){
    return (rand() % 100) / 10.0;
}

void affiche(float* tab, int n){
    for (int i = 0; i < n; i++){
        printf("%f ", tab[i]);
    }
    printf("\n");
}

void affichePoints(Point2d* tab, int n){
    for (int i = 0; i < n; i++){
        printf("(%f,%f) ", tab[i].x, tab[i].y);
    }
    printf("\n");
}

float *tabAlea(int n) {
    float *t = malloc(sizeof(float) * n);
    for (int i = 0; i < n; i++)
        t[i] = randf();
    return t;
}

Point2d *grilleAlea(int n, int m) {
    float *abs = tabAlea(n);
    float *ord = tabAlea(m);

    Point2d *pts = malloc(sizeof(Point2d) * n * m);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            Point2d p = { .x = abs[i], .y = ord[j] };
            pts[i + j * n] = p;
        }
    }

    free(abs);
    free(ord);
    return pts;
}

int main() {
    srand((unsigned int)time(NULL));

    float *t = tabAlea(5);
    affiche(t, 5);
    free(t);

    Point2d *pts = grilleAlea(2, 3);
    affichePoints(pts, 2 * 3);
    free(pts);


    return 0;
}