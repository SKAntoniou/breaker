// for structs and bool
#include "prototypes.h"

char *list2string(node **x,int y) {
    // input is one linked string, one int for size needed for string, (needs to be calculated in advance).
    // setup memory for string
    char *str = malloc(y);
    if (str == NULL) {
        return NULL;
    }
    // just being extra safe
    str[0] = '\0';

    // list to string + clear list
    node *infclearlist = *x;
    while (infclearlist != NULL)
    {
        // Adding to the array part
        strcat(str,infclearlist->value);

        // freeing and move to next node part
        node *next = infclearlist->next;
        if (infclearlist->value != NULL) {
            free(infclearlist->value);
            infclearlist->value = NULL;
        }
        free(infclearlist);
        infclearlist = NULL;
        infclearlist = next;
    }
    *x = NULL;
    return str;
}

void clearlist(node **list) {
    node *templist = *list;
    while (templist != NULL)
    {
        node *next = templist->next;
        if (templist->value != NULL) {
            free(templist->value);
            templist->value = NULL;
        }
        free(templist);
        templist = NULL;
        templist = next;
    }
    *list = NULL;
}

int add2list(node **list, char *data, bool add2last) {
    // make new temp node for value
    node *n = malloc(sizeof(node));
    // if no memory
    if (n == NULL) {
        return 4;
    }
    // Add value to a new node
    int stringlength = strlen(data);
    n->value = malloc(sizeof(char) * (stringlength + 1));
    if (n->value == NULL) {
        return 4;
    }
    strcpy(n->value,data);

    // make temp node to scroll to last
    node *last = *list;

    if (add2last) {
        n->next = NULL;
        // Add value node to the end of list
        // Check if list is empty / this is the first node
        if (*list == NULL) {
            *list = n;
        } else {
            // scroll until hits last node
            while (last->next != NULL) {
                last = last->next;
            }
            // add value node to the end of list
            last->next = n;
        }
    } else {
        // Add value to start of list
        n->next = *list;
        *list = n;
    }
    return 0;
}

int convert_time_and_print(time_t input){
    int sec2min = 60;
    int sec2hours = 60 * 60;
    int sec2days = 60 * 60 * 24;
    int days, hours, mins, sec;
    days = input / sec2days;
    sec = input - (days * sec2days);
    hours = sec / sec2hours;
    sec = sec - (hours * sec2hours);
    mins = sec / sec2min;
    sec = sec - (mins * sec2min);
    // 000:00:00:00 format, three for days, two for everthing else. means I need 13 char worth of memory. If it overflows, the overflowing chars won't be in the printed output. That would mean the program has to be running for about 3 years so all good.
    char *formatted = malloc(sizeof(char) * 13);
    if (formatted == NULL) {
        return 1;
    }
    for (int i = 0; i < 3; i++) {
        formatted[2 - i] = days % 10 + '0';
        days = days / 10;
    }
    formatted[3] = ':';
    for (int i = 0; i < 2; i++) {
        formatted[5 - i] = hours % 10 + '0';
        hours = hours / 10;
    }
    formatted[6] = ':';
    for (int i = 0; i < 2; i++) {
        formatted[8 - i] = mins % 10 + '0';
        mins = mins / 10;
    }
    formatted[9] = ':';
    for (int i = 0; i < 2; i++) {
        formatted[11 - i] = sec % 10 + '0';
        sec = sec / 10;
    }
    formatted[12] = '\0';
    // printing and freeing here to not have manage this memory anywhere else.
    printf("%s\n",formatted);
    free(formatted);
    return 0;
}

void preprocess_cyphers(function_struct *cyphers, int numofcyphers, function_struct *cyphers2, int numofcyphers2) {
    // Check if cyphers to be used was specified by user.
    bool usercyphers = false;
    for (int i = 0; i < numofcyphers; i++) {
        if (cyphers[i].enabled) {
            usercyphers = true;
        }
    }
    for (int i = 0; i < numofcyphers2; i++) {
        if (cyphers2[i].enabled) {
            usercyphers = true;
        }
    }
    // set all cyphers to be enabled if user did not do anything.
    if (!usercyphers) {
        for (int i = 0; i < numofcyphers; i++) {
            cyphers[i].enabled = true;
        }
        for (int i = 0; i < numofcyphers2; i++) {
            cyphers2[i].enabled = true;
        }
    }
    return;
}

int count_cyphers(function_struct *cyphers, int numofcyphers) {
    int cypherscount = 0;
    for (int i = 0; i < numofcyphers; i++) {
        if (cyphers[i].enabled == true) {
            cypherscount++;
        }
    }
    return cypherscount;
}

cypher_options_struct *list_traverse(cypher_options_struct *options,int target_layer) {
    // if you input a target layer above or equal to max_layer then this will cause a segmentation error but user of program can't do that (wihtout editing the code), it would have to be a bug.
    if (options == NULL) {
        return options;
    }
    while (options->layer != target_layer) {
        if (options->layer > target_layer) {
            if (options->prev != NULL){
                options = options->prev;
            } else {
                printf("boundary error in list_traverse function\n");
                return options;
            }
        } else if (options->layer < target_layer) {
            if (options->next != NULL){
                options = options->next;
            } else {
                printf("boundary errorin list_traverse function\n");
                return options;
            }
        }
    }
    return options;
}

