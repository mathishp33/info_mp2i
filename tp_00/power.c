#include <stdio.h>

int power(int x, int y) {
    //x^y
    int r = 1;

    if (!y)
        return r;

    while (y) {
        if (y % 2 == 1) {
            r *= x;
            y -= 1;
        }
        r *= x * x;
        y /= 2;
    }
    return r;
}

int main() {
    printf("\n ------------ POWER ------------ \n");
    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 6; j++)
            printf("power(%d, %d)= %d \n", i, j, power(i, j));

    return 0;
}