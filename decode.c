#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "io.h"
#include "node.h"
#include "code.h"
#include "stack.h"
#include "defines.h"
#include "huffman.h"
#include "header.h"

uint64_t bytes_read1 = 0;
uint64_t bytes_written1 = 0;

void usage();

int main(int argc, char *argv[]) {
    int opt = 0;
    char *in_file;
    char *out_file;
    int infile = STDIN_FILENO;
    int outfile = STDOUT_FILENO;

    bool verbose = false;
    bool inf = false;
    bool outf = false;

    while ((opt = getopt(argc, argv, "i:o:vh")) != -1) {
        switch (opt) {
        case 'i':
            in_file = optarg;
            inf = true;
            break;
        case 'o':
            out_file = optarg;
            outf = true;
            break;
        case 'v': verbose = true; break;
        case 'h': usage(); return 1;
        } //END switch
    } //END getopt()
    if (inf == true) { //If user entered an input file, open it. Else, read from stdin
        infile = open(in_file, O_RDONLY);
        if (infile == -1) { //Checking to see if input file opens/exists
            perror("Couldn't open infile\n");
            return 1;
        }
    }

    if (outf == true) { //If user entered an output file, open it. Else, print to stdout
        outfile = open(out_file, O_CREAT | O_TRUNC | O_WRONLY, 0600);
    }

    uint8_t *head_in = (uint8_t *) malloc(sizeof(Header));
    bytes_read1 += read_bytes(infile, head_in, sizeof(Header));

    Header *head = (Header *) head_in; //Read and cast the header to header struct

    if (head->magic != MAGIC) { //Verify file magic number
        printf("\nFile's Magic Number is Incorrect!\n");
    }

    fchmod(outfile, head->permissions); //Permissions for outfile

    uint8_t *tree = (uint8_t *) malloc(head->tree_size);
    bytes_read1 += read_bytes(
        infile, tree, head->tree_size); //Read dumped tree that is tree_size bytes long
    Node *root = rebuild_tree(head->tree_size, tree);
    Node *pointer = root;

    uint64_t symbol_found = 0;
    uint8_t bit;
    int counter = 0;
    while (symbol_found != head->file_size) { //Read every bit in infile, one at a time
        read_bit(infile, &bit);
        if (bit == 0) { //If the bit is a 0, point to the left child
            pointer = pointer->left;
            if (pointer->left == NULL
                && pointer->right == NULL) { //If the child is a leaf, write its symbol to outfile
                write_bytes(outfile, &pointer->symbol, 1);
                pointer = root; //Reset pointer to root
                symbol_found++;
                bytes_written1++;
            }
        }
        if (bit == 1) { //If the bit is a 0, point to the right child
            pointer = pointer->right;
            if (pointer->left == NULL
                && pointer->right == NULL) { //If the child is a leaf, write its symbol to outfile
                write_bytes(outfile, &pointer->symbol, 1);
                pointer = root; //Reset pointer to root
                symbol_found++;
                bytes_written1++;
            }
        }
        counter += 1;
    }

    bytes_read1 += (counter / 8);
    float bytes_read2 = bytes_read1;
    float bytes_written2 = bytes_written1;

    close(infile);
    close(outfile);

    if (verbose == true) { //Verbose enabled
        float space = 100 * (1 - (bytes_read2 / bytes_written2));
        printf("Compressed file size: %.0f bytes\n", bytes_read2);
        printf("Decompresssed file size: %.0f bytes\n", bytes_written2);
        printf("Space saving: %0.2f%%\n", space);
    }
}
void usage(void) {
    printf("SYNOPSIS\n"
           "A Huffman decoder.\n"
           "Decompresses a file using the Huffman coding algorithm.\n"

           "USAGE\n"
           "  ./decode [-h] [-i infile] [-o outfile]\n"

           "OPTIONS\n"
           "  - h       Program usage and help.\n"
           "  - v       Print compression statistics.\n"
           "  - i infile    Input file to decompress.\n"
           "  - o outfile   Output of decompressed data.\n");
}
