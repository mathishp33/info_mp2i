#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


typedef struct Particule_ {
    int id;
    float x;
    float y;
    float vx;
    float vy;
} Particule;


float randf10(){
    return (float)(rand() % 200 - 100) / 20.0;
}

float randf1(){
    return (float)(rand() % 20 - 10) / 20.0;
}

Particule *creeParticule(int id) {
    Particule *ptr = malloc(sizeof(Particule));
    ptr->id = id;
    ptr->x = randf10();
    ptr->y = randf10();
    ptr->vx = randf1();
    ptr->vy = randf1();
    return ptr;
}

void afficheParticule(Particule *p) {
    printf("particule = {");
    printf("id = %d", p->id);
    printf(", x = %f", p->x);
    printf(", y = %f", p->y);
    printf(", vx = %f", p->vx);
    printf(", vy = %f }\n", p->vy);
}

void deplaceParticule(Particule *p, float dt) {
    p->x += p->vx * dt;
    p-> y += p->vy * dt;
}

void detruitParticule(Particule *p) {
    free(p);
}

int main() {
    srand((unsigned int)time(NULL));

    Particule* p = creeParticule(122);
    for (int i = 0; i < 10; i++) {
        deplaceParticule(p, 0.5);
        afficheParticule(p);
    }
    detruitParticule(p);

    return 0;
}