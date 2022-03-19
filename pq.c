#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "pq.h"

void fix_heap(PriorityQueue *q, uint32_t min);

struct PriorityQueue {
    Node **array;
    uint32_t size;
    uint32_t elements;
};

//Constructor fucntion that creates the priority queue
//First parameter: Priority queue's maximum capacity
PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *pq = (PriorityQueue *) malloc(sizeof(PriorityQueue));
    Node **array = (Node **) malloc(capacity * sizeof(Node *));
    pq->array = array;
    pq->size = capacity;
    pq->elements = 0;
    return pq;
}

//Destructor function that deletes the priority queue
//First parameter: Pointer to a priority queue
void pq_delete(PriorityQueue **q) {
    free((*q)->array);
    free(*q);
    *q = NULL;
    return;
}

//Accessor function that returns whether the priority queue is empty
//First parameter: Pointer to a priority queue
bool pq_empty(PriorityQueue *q) {
    if (pq_size(q) == 0) { //If # of elements in queue is 0 it is empty
        return true;
    } else {
        return false;
    }
}

//Acessor function that returns whether the priority queue is full
//First parameter: Pointer to a priority queue
bool pq_full(PriorityQueue *q) { //If the # of elements in the queue == capacity, the queue is full
    if (pq_size(q) == q->size) {
        return true;
    } else {
        return false;
    }
}

//Accessor function that returns the size of the priority queue
//First parameter: Pointer to a priority queue
uint32_t pq_size(PriorityQueue *q) {
    return q->elements;
}

//Manipulator function that enqueues a node in a priority queue, returns false if the queue is full prior to enqueuing, true otherwise
//First parameter: Pointer to a priority queue
//Second parameter: Pointer to a node
bool enqueue(PriorityQueue *q, Node *n) {
    if (pq_full(q) == true) {
        return false;
    }
    q->elements = q->elements + 1; //Increase # of elements in queue
    q->array[pq_size(q) - 1] = n;
    uint32_t i = pq_size(q) - 1;

    while (i > 0
           && q->array[i / 2]->frequency
                  > q->array[i]->frequency) { //Cite: Intro to Algorithms & Geeksforgeeks.org
        Node *temp = q->array[i];
        q->array[i] = q->array[i / 2];
        q->array[i / 2] = temp; //Swap current node and parent node
        i = i / 2;
    }
    return true;
}

//Manipulator function that dequeues a node that is passed back through n
//First parameter: Pointer to a priority queue
//Second parameter: Double pointer to a node
bool dequeue(PriorityQueue *q, Node **n) { //Cite: Intro to Algorithms
    if (pq_empty(q) == true) {
        return false;
    }
    *n = q->array[0];
    q->array[0] = q->array[pq_size(q) - 1];
    q->elements = q->elements - 1;
    fix_heap(q, 0);
    return true;
}

void pq_print(PriorityQueue *q) {
    printf("EMPTY: %d\n", pq_empty(q));
    printf("FULL: %d\n", pq_full(q));
    printf("SIZE: %u\n", pq_size(q));
    for (uint32_t i = 0; i < pq_size(q); i++) {
        node_print(q->array[i]);
        printf("\n");
    }
    return;
}

//Helper function used to fix the heap after a node is dequeued
void fix_heap(PriorityQueue *q, uint32_t i) { //Cite: Intro to Algorithms/geeksforgeeks.org
    uint32_t min;

    if ((2 * i) < pq_size(q) && q->array[(2 * i)]->frequency < q->array[i]->frequency) {
        min = (2 * i);
    } else {
        min = i;
    }

    if (((2 * i) + 1) < pq_size(q) && q->array[(2 * i) + 1]->frequency < q->array[min]->frequency) {
        min = ((2 * i) + 1);
    }

    if (min != i) {
        Node *temp = q->array[min];
        q->array[min] = q->array[i];
        q->array[i] = temp;
        fix_heap(q, min);
    }
}
