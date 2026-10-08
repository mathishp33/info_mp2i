#include <stdio.h>

void hanoi(unsigned int n, unsigned int dep, unsigned int arr, unsigned int inter) {
    if (!n)
        return ;

    hanoi(n - 1, dep, inter, arr);
    printf("%d -> %d \n", arr, dep);
    hanoi(n - 1, inter, arr, dep);

}

int main() {
    printf("\n ------------ HANOI ------------ \n");
    hanoi(3, 0, 2, 1);

    return 0;
}