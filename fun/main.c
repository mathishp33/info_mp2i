#include <stdio.h>
#include <stdlib.h>


struct Ptr {
    int* ptr;
};


struct Ptr my_malloc(long unsigned int size) {
    struct Ptr ptrs = Ptr();
    if (!size) {
        ptrs.ptr = NULL;
        return ptrs;
    }

    int* ptr = malloc(size);
    if (ptr == NULL)
        printf("NULL PTR: Dynamic Allocation Error \n");
    else
        printf("Memory Successfully Allocated \n");
    ptrs.ptr = ptr;
    return ptrs;
}

int check_ptr(Ptr ptr) {
    if (ptr == NULL) {
        printf("NULL PTR");
        return 0;
    }
    return 1;
}

void get_ref(Ptr ptr) {
    return ptr.ptr;
}

void free(Ptr ptr) {
    if (ptr == NULL) {
        printf("NULL PTR");
        return ;
    }
    free(ptr);
}

int main() {
    Ptr my_ptr = my_malloc(4 * 5);
    check_ptr(my_ptr);


    return 0;
}