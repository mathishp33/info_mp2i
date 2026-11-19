#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main() {
    int *t;
    printf("%d \n", *t);

    t = malloc(sizeof(int) * 5);
    t[0] = 27; t[1] = 45; t[2] = 33; t[3] = 11; t[4] =  57;
    printf("%d \n", t[5]);
    printf("%d \n", t[99]);

    printf("t = \n");
    for (int i = 0; i < 5; i++)
        printf("%d \n", t[i]);

    free(t);
    printf("t = \n");
    for (int i = 0; i < 5; i++)
        printf("%d \n", t[i]);

    free(t);
    free(t);
    printf("t = \n");
    for (int i = 0; i < 5; i++)
        printf("%d \n", t[i]);

    return 0;
}