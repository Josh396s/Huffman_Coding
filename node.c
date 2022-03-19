#include "node.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

//Constructor function that creates the node
//First Parameter: Symbol of the node
//Second Parameter: Frequency of the symbol
Node *node_create(uint8_t symbol, uint64_t frequency) {
    Node *nd = (Node *) malloc(sizeof(Node));
    nd->symbol = symbol;
    nd->frequency = frequency;
    nd->left = NULL;
    nd->right = NULL;
    return nd;
}

//Destructor function that deletes the node
//First parameter: Pointer to pointer of void node_delete(Node **n);the node
void node_delete(Node **n) {
    free(*n);
    *n = NULL;
    return;
}

//Manipulator function that joins a left child node and right child node, returning a pointer to a created parent node
//First Parameter: Pointer to a left child node
//Second Parameter: Pointer to a right child node
Node *node_join(Node *left, Node *right) {
    uint64_t left_fr, right_fr, sum;
    left_fr = left->frequency;
    right_fr = right->frequency;
    sum = left_fr + right_fr;
    Node *parent = node_create('$', sum);
    parent->left = left;
    parent->right = right;
    return parent;
}

//Debugging function
//First parameter: Node
void node_print(Node *n) {
    printf("Symbol:%u\n", n->symbol);
    printf("Frequency: %lu\n", n->frequency);
    return;
}
