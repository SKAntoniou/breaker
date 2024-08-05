// Checks if this header is already loaded and makes sure it only gets loaded once.
#ifndef PROTOTYPE_H
#define PROTOTYPE_H

// If a library is used in multiple files, it will be included here instead.
#include <stdio.h>
// for printf, fopen, fclose, fprintf, fwrite
#include <stdbool.h>
// for bool type
#include <string.h>
// for strlen, strcpy and strcmp (unsafe if string is not null terminated, argv is null terminated)
#include <ctype.h>
// for isdigit, isalpha, isalnum & toupper
#include <time.h>
// for time_t & time (won't be as accurate as other methods but don't fraction of seconds accuracy and this is more portable)
#include <stdlib.h>
// for malloc & free

// Used for the dictionary, could save little bit of memory if decreased but mainly if you want to increase it.
#define MAX_WORD_LENGTH 45

// basic string linked list struct
typedef struct node {
    char *value;
    struct node *next;
} node;

// struct for cyphers
typedef struct function_struct {
    char *name;
    bool enabled;
} function_struct;

// struct for cypher types
typedef struct cypher_options_struct {
    int layer;
    bool reversed;
    int skips;
    char *keyword;
    char *cypher_used;
    char *solution;
    char *alphabet;
    struct cypher_options_struct *prev;
    struct cypher_options_struct *next;
} cypher_options_struct;

typedef struct {
    char letter;
    int count;
} alphabet_frequency_struct;

// helpers
char *list2string(node **x,int y);
void clearlist(node **list);
int add2list(node **list, char *data, bool add2last);
int convert_time_and_print(time_t input);
void preprocess_cyphers(function_struct *cyphers, int numofcyphers, function_struct *cyphers2, int numofcyphers2);
int count_cyphers(function_struct *cyphers, int numofcyphers);
cypher_options_struct *list_traverse(cypher_options_struct *main,int target_layer);

// cypher functions
char *f_reverse(char *input);
char *f_skip(char *input, int skips);
char *f_vigenere(char *input, char *alphabet, char *keyword);

// testers functions
int f_index_of_coincidence(char *input, bool keylength_check);
int f_frequency(char *input);
int f_matchesSolved(char *input, char *solved, int offset);

#endif
