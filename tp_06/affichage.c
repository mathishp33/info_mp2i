#include <unistd.h>
#include <stdio.h>

void rebours(int n) {
    if (n <= 0)
        return ;
    for (int i = n; i > 0; i--) {
        printf("T-%d \n", i);
        sleep(1);
    }
    printf("T-0 \n");
}

void coupe(int n) {
    for (int i = n; i >= 1; i /= 2) {
        printf("=%d \n", i);
        sleep(1);
    }
}


int main() {
    //rebours(10);

    coupe(175);

    return 0;
}