#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdint.h>

#include "code.h"

//Constructor function that creates the code
Code code_init(void) {
    Code code;
    code.top = 0;
    for (int i = 0; i <= MAX_CODE_SIZE; i++) { //Zeoring out the array of bits
        code.bits[i] = 0;
    }
    return code;
}

//Accessor function that returns the size of the code
//First parameter: Pointer to the code
uint32_t code_size(Code *c) {
    return c->top;
}

//Accessor function that returns if the code is empty
//First parameter: Pointer to the code
bool code_empty(Code *c) {
    if (c->bits[0] == 0 && c->top == 0) { //If the first bit is 0 and top points to byte 0
        return true;
    } else {
        return false;
    }
}

//Accessor function that returns if the code is full
//First parameter: Pointer to the code
bool code_full(Code *c) {
    if (c->top > ALPHABET) { //If top is more than 256 bits
        return true;
    } else {
        return false;
    }
}

//Manipulator function that sets a bit at index i to 1
//First parameter: Pointer to the code
//Second parameter: Index of the bit
bool code_set_bit(Code *c, uint32_t i) {
    if (i < 0 || i > ALPHABET) { //If i is out of range, return false
        return false;
    }
    c->bits[i / 8] |= (0x00000001 << (i % 8)); //Cite: set.h asgn3
    return true;
}

//Manipulator function that clears a bit at index i to 0
//First parameter: Pointer to the code
//Second parameter: Index of the bit
bool code_clr_bit(Code *c, uint32_t i) {
    if (i < 0 || i > ALPHABET) { //If i is out of range, return false
        return false;
    }
    c->bits[i / 8] &= ~(0x1 << ((i % 8) & 0xFFFFFFFF)); //Cite: set.h asgn3
    return true;
}

//Acessor function that returns the bit at index i
//First parameter: Pointer to the code
//Second parameter: Index of the bit
bool code_get_bit(Code *c, uint32_t i) {
    if (i < 0 || i > ALPHABET) { //If i is out of range, return false
        return false;
    }
    uint32_t bitval
        = (c->bits[i / 8] & (0x00000001 << (i % 8)))
          != 0; //Cite: https://stackoverflow.com/questions/44978126/structure-for-an-array-of-bits-in-c
    if (bitval == 1) { //If the bits value is 1, return true
        return true;
    } else {
        return false;
    }
}

//Manipulator function that pushes a bit onto the code
//First parameter: Pointer to the code
//Second parameter: Value of the bit
bool code_push_bit(Code *c, uint8_t bit) {
    if (code_full(c) == true) {
        return false;
    }
    c->bits[c->top / 8] |= (bit << (c->top % 8)); //Insert the bit into the array
    c->top++; //Move top by 1
    return true;
}

//Manipulator function that pops a bit from the code
////First parameter: Pointer to the code
////Second parameter: Pointer to the bit
bool code_pop_bit(Code *c, uint8_t *bit) {
    if (code_empty(c) == true) {
        return false;
    }
    c->top--; //Decrement the top by 1
    *bit = code_get_bit(c, c->top);
    c->bits[c->top / 8] &= ~(0x1 << ((c->top % 8) & 0xFFFFFFFF)); //Remove the bit from the array
    return true;
}

//Debuging function
void code_print(Code *c) {
    printf("\nTop: %u\n", c->top);
    printf("Size:%u\n", code_size(c));
    printf("Empty:%d\n", code_empty(c));
    printf("Full:%d\n", code_full(c));
    for (int i = 0; i <= MAX_CODE_SIZE; i++) {
        printf("%u, ", c->bits[i]);
    }
    printf("\n");
}
