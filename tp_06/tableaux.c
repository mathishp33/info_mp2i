#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void afficheTab(const int *ptr, size_t n, const char *name) {
    if (ptr == NULL)
        return ;

    printf("%s = { ", name);
    for (size_t i = 0; i < n; i++) {
        printf("%d", ptr[i]);
        if (i != n - 1)
            printf(", ");
    }
    printf(" } \n");

}

void reverse(int *ptr, size_t n) {
    if (ptr == NULL)
        return ;
    int temp;
    for (size_t i = 0; i < n / 2; i++) {
        temp = ptr[i];
        ptr[i] = ptr[n - i - 1];
        ptr[n - i - 1] = temp;
    }
}

#ifndef __S__
#define __S__ 10
#endif

int main() {
    int tab1[__S__ / 2] = { 0, 1, 2, 4, 5 };  // int *tab1 = malloc(5 * sizeof(int));
    int tab2[__S__];
    memcpy(tab2, tab1, __S__ / 2 * sizeof(int));
    memcpy(tab2 + __S__ / 2, tab1, __S__ / 2 * sizeof(int));

    afficheTab(tab1, __S__, "tab1");
    afficheTab(tab2, __S__, "tab2");

    reverse(tab2, __S__);
    afficheTab(tab2, __S__, "tab2");


    return 0;
}