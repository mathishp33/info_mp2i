#include <stdio.h>

unsigned int fact_rec(unsigned int n) {
    if (n == 0)
        return 0;
    if (n == 1)
        return 1;
    return fact_rec(n - 1) * n;
}


unsigned int fact_it(unsigned int n) {
    unsigned int fact = 1;
    for (unsigned int i = 0; i < n; i++) {
        fact *= i + 1;
    }
    return fact;
}

int main() {
    printf("\n ------------ FACT ------------ \n");
    for (int i = 0; i < 13; i++)
        printf("fact_it(%d)= %d \n", i, fact_it(i));
    for (int i = 0; i < 13; i++)
        printf("fact_rec(%d)= %d \n", i, fact_rec(i));

    return 0;
}