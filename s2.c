#include <stdio.h>
#include <stdlib.h>

// Intercambuar
void ej2(int* a, int* b){
    int buffer = *a;
    *a = *b;
    *b = buffer;
    return;
}

void ej3(char string[12]){
    int arr[10];
    int size = sizeof(arr);
    printf("El array es de tamaño: %i\n", size);

    char string_2[8];

    for(int i=0;i<8;i++){
        string_2[i] = string[i];
    }
    size = sizeof(string_2);

    printf("El string es de tamaño: %i\n", size);
    
    return;
}

char str[12] = "Hola";
void (*ptr[2])() = {ej2, ej3}; 

int main() {
    ptr[1](str);
    
    return 0;
}
