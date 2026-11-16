/* manips1.c */
    
#include <stdio.h>
    

int main(){
    int x = 2; int y = 1000;
    int *px = &x; int *py = &y;
    int *p = &x;
    *p *= 2;
    *py += *p;

    printf("x = %d; y = %d \n", x, y);
    return 0;
}

