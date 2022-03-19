#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "io.h"
#include "code.h"
#include "defines.h"

uint64_t bytes_read = 0;
uint64_t bytes_written = 0;
uint64_t bytes_read_tracker;
uint64_t bytes_write = 0;
static uint8_t buf_c[BLOCK];
size_t index_c = 0;

//Reads a file and fills the buffer with bytes read
int read_bytes(int infile, uint8_t *buf, int nbytes) {
    int total = 0;
    while ((bytes_read_tracker = read(infile, buf + total, nbytes))
           > 0) { //Read until there are no more bytes
        total += bytes_read_tracker;
        bytes_read += bytes_read_tracker;
        if (total == nbytes) {
            return total;
        }
    }
    return total;
}

//Takes a buffer and writes out its content to outfile
int write_bytes(int outfile, uint8_t *buf, int nbytes) {
    int left = 0;
    while ((bytes_write = write(outfile, buf + left, nbytes))
           > 0) { //Write until there are no more bytes
        left += bytes_write;
        bytes_written += bytes_write;
        if (left == nbytes) {
            break;
        }
    }
    return left;
}

//Reads the bits from a infile and returns a single bit
bool read_bit(int infile, uint8_t *bit) {
    static uint8_t buf[BLOCK];
    static uint32_t index = 0;
    int bytes;
    uint32_t last_bit;

    if (index == 0) { //Cite: Eugene Office Hours 2/25
        bytes = read_bytes(infile, buf, BLOCK);
        if (bytes < BLOCK) { //If there are leftover bits, end once you get to it
            last_bit = (8 * bytes) + 1;
            if (index == last_bit) {
                return false;
            }
        }
    }
    if (index == 8 * sizeof(buf)) {
        index = 0;
    }
    *bit = ((buf[index / 8]) & (0x00000001 << (index % 8))) != 0;
    index++;
    return true;
}

//Takes in a code and prints its bits to outfile
void write_code(int outfile, Code *c) {
    uint32_t top = 0;
    while (top != code_size(c)) {
        int bit = code_get_bit(c, top);
        if (bit == 1) { //Set bit to 1 in buffer if it is 1 in code
            buf_c[index_c / 8] |= (1 << (index_c % 8));
        }
        index_c += 1;
        top += 1;
        if (index_c == (8 * sizeof(buf_c))) { //If buffer is full, print to outfile
            flush_codes(outfile);
        }
    }
    return;
}

//Function that prints out any remaining bits in the buffer to outfile
void flush_codes(int outfile) {
    if (index_c > 0) {
        int byte = (index_c + 7) / 8;
        write_bytes(outfile, buf_c, byte);
        memset(buf_c, 0, sizeof(buf_c)); //Zeroes out buffer(Cite: Eugene's office hours)
        index_c = 0;
    }
    return;
}
