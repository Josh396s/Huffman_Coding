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
#include "defines.h"
#include "huffman.h"
#include "header.h"

void stats();
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
    } else { //Taking care of STDIN
        int stdin_bytes = 0;

        if (lseek(STDIN_FILENO, 0, SEEK_SET) == -1) {

            int temp_file
                = open("tmp.dat", O_CREAT | O_TRUNC | O_RDWR, 0600); //CITE: Eugene Section
            unlink("tmp.dat");
            uint8_t *buffer = (uint8_t *) calloc(BLOCK, sizeof(uint8_t));

            while ((stdin_bytes = read(STDIN_FILENO, buffer, sizeof(buffer)))
                   > 0) { //Read stdin and write to output file
                write(temp_file, buffer, stdin_bytes);
            }
            lseek(temp_file, 0, SEEK_SET);
            close(infile);
            infile = temp_file;
        }
    }

    if (outf == true) { //If user entered an output file, open it. Else, print to stdout
        outfile = open(out_file, O_CREAT | O_TRUNC | O_WRONLY, 0600);
    }

    uint64_t *histogram = (uint64_t *) calloc(ALPHABET, sizeof(uint64_t)); //Actual histogram

    uint64_t *hist = (uint64_t *) calloc(ALPHABET,
        sizeof(uint64_t)); //Array with ascii characters (gonna be used to compare the input to)
    for (int ind = 0; ind < ALPHABET; ind++) {
        hist[ind] = ind;
    }

    uint8_t *input = (uint8_t *) calloc(BLOCK, sizeof(uint8_t)); //Buffer with input
    uint64_t bytes = read_bytes(infile, input, BLOCK);

    for (uint64_t i = 0; i < bytes; i++) { //Iterate over the input
        for (uint64_t index = 0; index < ALPHABET; index++) { //Iterate over the ascii characters
            if (input[i]
                == hist[index]) { //Increment the index of histogram by 1 at its proper ascii index
                histogram[index] += 1;
            }
        }
    }
    histogram[0] += 1;
    histogram[255] += 1;

    //CODE TABLE ---------------------------------------------------------------
    Node *root = build_tree(histogram); //Builds the huffman tree
    Code *table = (Code *) calloc(ALPHABET, sizeof(Code));
    build_codes(root, table);
    //END OF CODE TABLE --------------------------------------------------------

    //HEADER -------------------------------------------------------------------
    struct stat buf;
    fstat(infile, &buf);
    Header *h = (Header *) malloc(sizeof(Header));

    h->magic = MAGIC; //Magic Number (WORKS)
    h->permissions = buf.st_mode; //Input file permissions (WORKS)

    mode_t mode = buf.st_mode; //Outfile Permissions
    fchmod(outfile, mode);

    int unique = 0;
    for (uint64_t index = 0; index < ALPHABET; index++) { //Number of unique symbols
        if (histogram[index] != 0) {
            unique++;
        }
    }
    h->tree_size = (3 * unique) - 1; //Tree size

    size_t file_size = buf.st_size; //Set input file size
    h->file_size = file_size;

    uint8_t *header = (uint8_t *) h; //Write header to outfile
    write_bytes(outfile, header, 16);
    //END OF HEADER----------------------------------------------------------

    dump_tree(outfile, root); //Write Huffman tree to outfile

    lseek(infile, 0, SEEK_SET);
    for (uint32_t i = 0; i < file_size; i++) { //Iterate over the bytes in the file
        for (uint64_t index = 0; index < ALPHABET; index++) { //Iterate over the ascii characters
            if (input[i] == hist[index]) { //If its the symbol, print its code
                write_code(outfile, &table[index]);
            }
        }
    }
    bytes_read = h->file_size;
    bytes_written += h->tree_size;

    flush_codes(outfile);

    float bytes_read1 = bytes_read;
    float bytes_written1 = bytes_written;

    if (verbose == true) { //Verbose enabled
        float space = 100 * (1 - (bytes_written1 / bytes_read1));
        printf("Uncompressed file size: %.0f bytes\n", bytes_read1);
        printf("Compresssed file size: %.0f bytes\n", bytes_written1);
        printf("Space saving: %0.2f%%\n", space);
    }

    close(infile);
    close(outfile);

    free(h);
    free(table);
    delete_tree(&root);
    free(input);
    free(hist);
    free(histogram);
}

void usage(void) {
    printf("SYNOPSIS\n"
           "A Huffman encoder.\n"
           "Compresses a file using the Huffman coding algorithm.\n"

           "USAGE\n"
           "  ./encode [-h] [-i infile] [-o outfile]\n"

           "OPTIONS\n"
           "  - h       Program usage and help.\n"
           "  - v       Print compression statistics.\n"
           "  - i infile    Input file to compress.\n"
           "  - o outfile   Output of compressed data.\n");
}
