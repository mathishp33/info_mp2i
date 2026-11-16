/* calculette.c */

#include <stdio.h>
    
int main(){
    int x = 0; int y = 0;
    char op = "+";
    printf("saisir un calcul:\n");
    if (scanf("%d%c%d", &x, &op, &y) != 3) {
        printf("faute de frappe ! \n");
    }
    if (c == '+')
        printf("%d%c%d = %d\n", x, op, y, x + y);
    if (c == '-')
        printf("%d%c%d = %d\n", x, op, y, x - y);
    if (c == '*')
        printf("%d%c%d = %d\n", x, op, y, x * y);
    if (c == '/')
        printf("%d%c%d = %d\n", x, op, y, x / y);
    
    return 0;
}
