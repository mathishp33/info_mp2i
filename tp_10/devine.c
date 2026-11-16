/* devine.c */
    
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(){
    int nombre = rand() % 1001;
    int nombre_user = -1;
    bool commence = true;
    while (commence) {
        printf("devinez un nombre entre 0 et 1000 : \n");
        if (scanf("%d", &nombre_user) == 1) {
            if (nombre_user < nombre) {
                printf("trop bas \n");
            }
            else if (nombre_user > nombre) {
                printf("trop haut \n");
            }
        }
        else {
            printf("pas bon \n");
            commence = false;
        }
        if (nombre == nombre_user) {
            printf("gagné \n");
            commence = false;
        }
    }
    if (nombre != nombre_user)
        printf("perdu \n");

    return 0;
}
