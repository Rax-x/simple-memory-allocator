#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../include/allocator.h"

#define PRINT_TITLE(title) printf("\n\e[0;93m%s\e[0m\n", title)

void set_string(char* str, char value, int length){
    if(!str || length <= 0) return;
    while (length-- > 0) *str++ = value;
    *str = '\0';
}

void set_array(int* array, int length){
    srand(time(NULL));
    while (length-- > 0) *(array++) = (rand() % 0xFF)+1; 
}

void print_array(int* array, int length){
    while (length-- > 0) printf("%d ", *array++);
    putchar('\n');
}

void run_allocator_test1(){
    
    const int MAX_SIZE = 10;

    PRINT_TITLE("---- TEST 1 ----");

    char* str = (char*)_malloc(sizeof(char) * MAX_SIZE+1);
    char* str1 = (char*)_malloc(sizeof(char) * MAX_SIZE+1);

    set_string(str, 'A', MAX_SIZE);
    set_string(str1, 'C', MAX_SIZE);
    
    puts(str);
    puts(str1);
    
    _free(str);
    
    char* str2 = (char*)_malloc(sizeof(char) * MAX_SIZE+1);
    set_string(str2, 'D', MAX_SIZE);
    puts(str2);

    _free(str1);
    _free(str2);

    print_memory_blocks();
}


void run_allocator_test2(){
    
    const int MAX_SIZE = 5;

    PRINT_TITLE("---- TEST 2 ----");

    int* array1 = (int*)_malloc(sizeof(int) * MAX_SIZE);
    int* array2 = (int*)_malloc(sizeof(int) * MAX_SIZE);
    int* array3 = (int*)_malloc(sizeof(int) * MAX_SIZE);

    set_array(array1, MAX_SIZE);
    set_array(array2, MAX_SIZE);
    set_array(array3, MAX_SIZE);

    print_array(array1, MAX_SIZE);
    print_array(array2, MAX_SIZE);
    print_array(array3, MAX_SIZE);

    _free(array1);
    _free(array2);

    int *array4 = (int*)_malloc(sizeof(int) * (MAX_SIZE*2));

    set_array(array4, MAX_SIZE*2);
    print_array(array4, MAX_SIZE*2);

    _free(array4);
    _free(array3);

    print_memory_blocks();
}

void run_allocator_test3(){
    
    const int MAX_SIZE = 7;

    PRINT_TITLE("---- TEST 3 ----");

    int* array1 = (int*)_calloc(MAX_SIZE, sizeof(int));
    print_array(array1, MAX_SIZE);

    char* str = (char*)_malloc(sizeof(char) * MAX_SIZE+1);

    set_string(str, 'A', MAX_SIZE);
    printf("Initial string: %s\n", str);

    str = (char*)_realloc(str, MAX_SIZE+4);

    set_string(str, 'B', MAX_SIZE+3);
    printf("Reallocated string: %s\n", str);

    _free(str);
    _free(array1);

    print_memory_blocks();
}

int main(int argc, char **argv){
    
    run_allocator_test1();
    run_allocator_test2();
    run_allocator_test3();

    return 0;
}
