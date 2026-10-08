#include <stdio.h>
#include <stdlib.h>


typedef struct {
    int* ptr;
} Ptr;


Ptr my_malloc(long unsigned int size) {
    Ptr ptrs;
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
    if (ptr.ptr == NULL) {
        printf("NULL PTR");
        return 0;
    }
    return 1;
}

int* get_ref(Ptr ptr) {
    return ptr.ptr;
}

void my_free(Ptr ptr) {
    if (ptr.ptr == NULL) {
        printf("NULL PTR");
        return ;
    }
    free(ptr.ptr);
}

int main() {
    Ptr my_ptr = my_malloc(4 * 5);
    check_ptr(my_ptr);


    return 0;
}