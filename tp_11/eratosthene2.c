#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main() {
    int n = 0;
    printf("choisir n : ");
    if (scanf("%d", &n) == 1)
        printf("\nn = %d \n", n);
    else
        printf("oula la !!!");

    bool *t = malloc(sizeof(bool) * n);
    for (int i = 0; i < n; i++)
        t[i] = true;

    for (int i = 2; i < n; i++) {
        if (!t[i])
            continue;
        for (int j = i + i; j < n; j += i)
            t[j] = false;
    }

    printf("t = \n");
    for (int i = 0; i < n; i++)
        if (t[i])
            printf("%d premier\n", i);

    free(t);

    return 0;
}