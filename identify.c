#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "metric.h"
#include "text.h"
#include "pq.h"

void stats();
void usage();

int main(int argc, char *argv[]) {
    int opt = 0;
    char *db_file = "lib.db";
    char *noise_file = "noise.txt";
    FILE *dbfile;
    FILE *noisefile;
    uint32_t num_match = 5;
    uint32_t noise_lim;
    noiselimit = 100;
    char metric = EUCLIDEAN;

    bool dbf = false;
    bool noisef = false;
    bool euclid = false;
    bool manhattan = false;
    bool cosine = false;

    while ((opt = getopt(argc, argv, "d:n:k:l:emch")) != -1) {
        switch (opt) {
        case 'd':
            db_file = optarg;
            dbf = true;
            break;
        case 'n':
            noise_file = optarg;
            noisef = true;
            break;
        case 'k':
            num_match = atoi(optarg);
            uint32_t check = num_match;
            if (check == 0) {
                fprintf(stderr, "Please enter a valid number of matches \n");
                return 1;
            }
            break;
        case 'l':
            noise_lim = atoi(optarg);
            if (noise_lim == 0) {
                fprintf(stderr, "Please enter a valid number of noise words to filter \n");
                return 1;
            }
            noiselimit = noise_lim;
            break;
        case 'e': euclid = true; break;
        case 'm':
            manhattan = true;
            metric = MANHATTAN;
            break;
        case 'c':
            cosine = true;
            metric = COSINE;
            break;
        case 'h': usage(); return 1;
        } //END switch
    } //END getopt()
    if (noisef == true) {
        noisefile = fopen(noise_file, "r");
    } else {
        noisefile = fopen("noise.txt", "r"); //Open default noise.txt
    }

    Text *noise = text_create(noisefile, NULL);

    FILE *std_in = stdin;
    Text *anon = text_create(std_in, noise);

    if (dbf == true) { //If user entered an input file, open it
        dbfile = fopen(db_file, "r");
        if (dbfile == NULL) { //Checking to see if input file opens/exists
            fprintf(stderr, "Couldn't open infile\n");
            return 1;
        }
    } else { //Open defualt lib.d
        dbfile = fopen("lib.db", "r");
    }

    uint32_t n = 0;
    fscanf(dbfile, "%u", &n); //Get the first line of the data base
    PriorityQueue *pq = pq_create(n); //Priority Queue for authors

    char trash[50];
    fgets(trash, 50, dbfile); //Used to throw away the first line of the text

    char line[2000];
    uint32_t i = 0;

    while (i < n && !feof(dbfile)) {
        fgets(line, 496, dbfile); //Read in the author name
        if (strlen(line) == 0) {
            break;
        }
        line[strlen(line) - 1] = '\0';
        char *author = strdup(line);

        fgets(line, 496, dbfile); //Read in the path to file
        if (strlen(line) == 0) {
            break;
        }
        line[strlen(line) - 1] = '\0';

        FILE *author_text = fopen(line, "r");
        if (author_text == NULL) {
            printf("\nFILE COULD NOT BE OPEN\n");
            continue;
        }
        Text *text = text_create(author_text, noise);
        double result = text_dist(text, anon, metric);
        enqueue(pq, author, result);

        text_delete(&text);
        fclose(author_text);
        i++;
    }

    if (metric == MANHATTAN) { //If Manhattan is being used
        printf("Top %u, metric: Manhattan distance, noise limit: %u\n", num_match, noiselimit);
    }
    if (metric == COSINE) { //If cosine is being used
        printf("Top %u, metric: Cosine distance, noise limit: %u\n", num_match, noiselimit);
    }
    if (metric == EUCLIDEAN) { //If euclidean is being used
        printf("Top %u, metric: Euclidean distance, noise limit: %u\n", num_match, noiselimit);
    }

    char *author;
    double result = 0;
    for (uint32_t ind = 0; ind < num_match; ind++) {
        dequeue(pq, &author, &result);
        printf("%u) %s  ", ind + 1, author);
        printf("[%.15f]\n", result);
        free(author);
    }
    text_delete(&anon);
    text_delete(&noise);
    fclose(dbfile);
    fclose(std_in);
    fclose(noisefile);
    pq_delete(&pq);
}

void usage(void) {
    printf("SYNOPSIS\n"
           "Identifies the author of some anonymous text passed through stdin.\n"
           "Basically computes k-nearest neighbors, where k = 1 by default.\n"
           "The number of noise words that are removed is tunable.\n"
           "Uses either Euclidean, Manhattan, or cosine distance as metric.\n"

           "USAGE\n"
           "  ./identify [OPTIONS]\n"

           "OPTIONS\n"
           "  -d        Database of authors and texts [default: lib.db]\n"
           "  -k matches    Set number of top matches to display [default: 5]\n"
           "  -n noisefile  Set file of words to ignore [default: noise.txt]\n"
           "  -l limit      Set noise word limit [default: 100]\n"
           "  -e        Set distance metric as Euclidean distance [default]\n"
           "  -m        Set distance metric as Manhattan distance\n"
           "  -c        Set distance metric as cosine distance\n"
           "  -v        Enable verbose printing of program run\n"
           "  -h        Display program help and usage\n");
}
