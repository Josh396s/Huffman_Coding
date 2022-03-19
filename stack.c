#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "stack.h"

struct Stack {
    uint32_t top;
    uint32_t capacity;
    Node **items;
};

//Constructor function that creates the stack
//First parameter: Capacity of the stack
Stack *stack_create(uint32_t capacity) {
    Stack *stack = (Stack *) malloc(sizeof(Stack));
    if (stack != NULL) { //Stacks slide lecture
        stack->top = 0;
        stack->capacity = capacity;
        stack->items = (Node **) calloc(capacity, sizeof(Node *));
        if (stack->items == NULL) {
            free(stack);
            stack = NULL;
        }
    }
    return stack;
}

//Destructor function that deletes the priority queue
//First parameter: Pointer to a stack
void stack_delete(Stack **s) { //Cite: Stacks lecture
    if (s) {
        free((*s)->items);
        free(*s);
    }
    *s = NULL;
    return;
}

//Accessor function that returns whether the stack is empty
//First parameter: Pointer to a stack
bool stack_empty(Stack *s) {
    if (s->top == 0) {
        return true;
    } else {
        return false;
    }
}

//Accessor function that returns whether the stack is full
//First parameter: Pointer to a stack
bool stack_full(Stack *s) {
    if (s->top == s->capacity) {
        return true;
    } else {
        return false;
    }
}

//Accessor function that returns the stack size
//First parameter: Pointer to a stack
uint32_t stack_size(Stack *s) {
    return s->top;
}

//Manipulator function that pushes a node to the stack
//First parameter: Pointer to a stack
//Second parameter: Pointer to a node
bool stack_push(Stack *s, Node *n) {
    if (stack_full(s) == true) {
        return false;
    }
    s->items[s->top] = n; //Set the node being pointed at by top to n
    s->top++;
    return true;
}

//Manipulator function that pushes a node to the stack
//First parameter: Pointer to a stack
//Second parameter: Pointer to a pointer to a node
bool stack_pop(Stack *s, Node **n) {
    if (stack_empty(s) == true) {
        return false;
    }
    s->top--;
    *n = s->items[s->top]; //Set ponter to node to what top is pointing at
    return true;
}

//Debugging function
void stack_print(Stack *s) {
    printf("\nEmpty: %d\n", stack_empty(s));
    printf("Full: %d\n", stack_full(s));
    printf("Size: %u\n\n", stack_size(s));
    for (uint32_t i = 0; i < stack_size(s); i++) {
        node_print(s->items[i]);
        printf("\n");
    }
}
