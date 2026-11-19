#include <stdio.h>
#include <stdlib.h>


int main() {
    int *t = malloc(sizeof(int) * 5);
    t[0] = 27; t[1] = 45; t[2] = 33; t[3] = 11; t[4] =  57;

    printf("t = \n");
    for (int i = 0; i < 5; i++)
        printf("%d \n", t[i]);

    free(t);

    return 0;
}