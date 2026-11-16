/* manips2.c */
    
#include <stdio.h>
    


int main(){
    int tab[3] = { 20, 50, 90 };
    int *p2 = &tab[2];
    *p2 /= 2;
    for (int i = 0; i < 3; i++)
        printf("x = %d; &x = %p \n", tab[i], &tab[i]);

    return 0;
}

