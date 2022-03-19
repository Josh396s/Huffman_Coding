#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "huffman.h"
#include "node.h"
#include "pq.h"
#include "code.h"
#include "stack.h"

//Constructs a Huffman tree using a priority queue, returns a node
Node *build_tree(uint64_t hist[static ALPHABET]) {
    PriorityQueue *que = pq_create(ALPHABET); //Create a priority queue

    for (uint64_t index = 0; index < ALPHABET; index++) { //Create nodes for the histogram
        if (hist[index] != 0) {
            Node *node = node_create(index, hist[index]);
            enqueue(que, node);
        }
    }

    Node *left;
    Node *right;
    while (pq_size(que) > 1) {
        dequeue(que, &left);
        dequeue(que, &right);
        Node *parent = node_join(left, right);
        enqueue(que, parent);
    }
    Node *root;
    dequeue(que, &root);

    return root;
}

static Code code;
uint8_t bit;
//Populates a code table with codes for each symbol in the Huffman tree
void build_codes(Node *root, Code table[static ALPHABET]) {
    if (code_empty(&code)) {
        code = code_init();
    }

    if (root != NULL) {
        if (root->left == NULL && root->right == NULL) {
            table[root->symbol] = code;
        } else {
            if (root->left != NULL) {
                code_push_bit(&code, 0);
                build_codes(root->left, table);
                code_pop_bit(&code, &bit);
            }
            if (root->right != NULL) {
                code_push_bit(&code, 1);
                build_codes(root->right, table);
                code_pop_bit(&code, &bit);
            }
        }
    }
}

uint8_t symbol[1];
//Does a post-order traversal of the Huffman tree and writes it to outfile
void dump_tree(int outfile, Node *root) {
    symbol[0] = (root->symbol);
    if (root != NULL) {
        if (root->left != NULL) { //If there is a left child, move to that child
            dump_tree(outfile, root->left);
        }
        if (root->right != NULL) { //If there is a right child, move to that child
            dump_tree(outfile, root->right);
        }
        if (root->left == NULL
            && root->right == NULL) { //If the root is a leaf, right 'L' to outfile
            write(outfile, "L", 1);
            write(outfile, symbol, 1);
        } else {
            write(outfile, "I", 1); //Write 'I' to outfile
        }
    }
}

static int ind = 0;
//Reconstructs the Huffman tree
Node *rebuild_tree(uint16_t nbytes, uint8_t tree_dump[static nbytes]) {
    Stack *s = stack_create(nbytes);
    while (ind != nbytes) {
        if (tree_dump[ind] == 73) { //If element is 'I'
            Node *left;
            Node *right;
            stack_pop(s, &right);
            stack_pop(s, &left);
            Node *parent
                = node_join(left, right); //Pop and join two nodes, push joined node to stack
            stack_push(s, parent);
        }
        if (tree_dump[ind] == 76) { //If element is 'L'
            Node *symbol = node_create(tree_dump[ind + 1], 0);
            stack_push(s, symbol);
            ind++;
        }
        ind += 1;
    }
    Node *root;
    stack_pop(s, &root);
    return root;
}

//Deletes the huffman tree through post order traversal
void delete_tree(Node **root) {
    if (*root) {
        node_delete(&(*root)->left);
        node_delete(&(*root)->right);
        node_delete(root);
    }
    *root = NULL;
}