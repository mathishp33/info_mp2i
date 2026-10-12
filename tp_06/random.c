#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>

float randf() {
    return (int)((float)rand() / (float)INT_MAX * 1e3) / 1e3;
}

int experience() {
    float x = randf(), y = randf();
    return x * x + y * y <= 1;
}

float approx_pi(int n) {
    int res = 0;
    for (int i = 1; i < n + 1; i++)
        res += experience();
    return (float)res / (float)n * 4;
}


int main() {
    for (int i = 1e3; i <= 1e6; i *= 10)
        printf("n = %d, pi = %f \n", i, approx_pi(i));

    return 0;
}