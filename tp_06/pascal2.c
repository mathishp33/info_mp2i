#include <stdlib.h>
#include <stdio.h>


int fact(int n) {
    int f = 1;
    for (int i = 1; i < n + 1; i++)
        f *= i;
    return f;
}

int binom(int x, int y) {
    if (x < 0 || y < 0)
        return 0;
    return fact(x) / (fact(y) * fact(x - y));
}

void triangle_pascal(int *t, size_t n) {
    t[0] = 1;
    printf("1 ");
    for (size_t i = 0; i <= n; i++) {
        for (size_t j = i; j > 0; j--) {
            t[j] = t[j - 1] + t[j];
            printf("%d ", t[j]);
        }
        if (i) {
            t[i + 1] = 1;
            printf("1 ");
        }
        printf("\n");
    }
}


#ifndef __S__
#define __S__ 10
#endif

int main() {
    int t[__S__ + 1] = { 1 };

    triangle_pascal(t, __S__ + 1);

    return 0;
}