#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <assert.h>

#ifndef __S__
#define __S__ 10
#endif

typedef struct Point2D {
    float x;
    float y;
} point2D;


void affiche(point2D p) {
    printf("P(x = %f, y = %f); \n", p.x, p.y);
}

point2D milieu(point2D p, point2D q) {
    point2D r = { .x = 0.0f, .y = 0.0f };

    r.x = (p.x + q.x) / 2;
    r.y = (p.y + q.y) / 2;
    return r;
}

float randf10() {
    return (int)((float)rand() / (float)INT_MAX * 1e1) / 1e1 * 20 - 10;
}

point2D pointAleatoire() {
    point2D r = { .x = randf10(), .y = randf10() };
}

float dist2(point2D p, point2D q) {
    return (p.x - q.x) * (p.x - q.x) + (p.y - q.y) * (p.y - q.y);
}

point2D pointleplusproche(point2D pts[], size_t n, point2D p) {
    assert(n);
    
    float d = dist2(pts[1], p);
    size_t idx = 0;
    for (size_t i = 1; i < n; i++) {
        float nd = dist2(pts[i], p);
        if (nd < d) {
            d = nd;
            idx = i;
        }
    }
    return pts[idx];
}


int main() {
    point2D p1 = { .x = 3.5f, .y = 0.5f };
    point2D p2 = { .x = 1.5f, .y = 7.5f };
    affiche(p1);
    affiche(p2);

    point2D m = milieu(p1, p2);
    affiche(m);

    point2D pts[__S__];
    for (size_t i = 0; i < __S__; i++)
        pts[i] = pointAleatoire();
    point2D plpp = pointleplusproche(pts, __S__, p2);
    affiche(plpp);

    return 0;
}