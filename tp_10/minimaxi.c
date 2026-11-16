/* minimaxi.c */
    
#include <stdio.h>
    
void minimaxi(int tab[], int n, int* mini, int* maxi) {
    if (!n) return ;
    *mini = tab[0];
    *maxi = tab[0];
    for (int i = 1; i < n; i++) {
        if (tab[i] < *mini)
            *mini = tab[i];
        if (tab[i] > *maxi)
            *maxi = tab[i];
    }
}
    
int main(){
    int T[5] = {0,4,2,7,-1};
    int maxi; int mini;
    minimaxi(T, 5, &mini, &maxi);
    printf("min = %d; maxi = %d \n", mini, maxi);
    return 0;
}

