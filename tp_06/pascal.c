#include <stdio.h>

int fact(int n) {
    int f = 1;
    for (int i = 1; i < n + 1; i++)
        f *= i;
    return f;
}

int binom(x, y) {
    if (x <= 0 || y <= 0)
        return 0;
    return fact(x) / (fact(y) * fact(x - y));
}


void triangle_pascal(int x, int prof_max) {
    if (prof_max <= 0)
        return ;
    if (x > prof_max)
        return ;

    for (int y = 1; y < x + 2; y++)
        printf("%d  ", binom(x, y));
    triangle_pascal(++x, prof_max);
    
    printf("\n");
}


int main() {
    triangle_pascal(1, 12);

    return 0;
}