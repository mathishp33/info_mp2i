#include <stdbool.h>
#include <stdio.h>

bool est_pythagoricien(int a, int b, int c) {
    return a * a + b * b == c * c;
}

void triplets_pytha(int from_, int to_) {
    int total = 0;
    for (int a = from_; a < to_; a++)
        for (int b = a; b < to_; b++)
            for (int c = b; c < to_; c++) {
                bool res = est_pythagoricien(a, b, c);
                if (res) {
                    printf("a = %d, b = %d, c = %d | %d \n", a, b, c, (int)res);
                    total += 1;
                }
            }
    printf("total = %d \n", total);
}

int main() {
    triplets_pytha(1, 101);

    return 0;
}