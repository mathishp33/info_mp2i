#include <stdio.h>


int est_premier(unsigned int n) {
    unsigned int i = 2;

    while (i < n / 2 + 1) {
        if (!(n % i))
            return 0;
        i += 2;
    }
    return 1;
}

void premiers(unsigned int n) {
    for (unsigned int i = 2; i < n; i++)
        if (est_premier(i))
            printf("%d est premier \n", i);
}


int main() {
    printf("\n ------------ PREMIEROPT ------------ \n");
    
    premiers(100);

    return 0;
}