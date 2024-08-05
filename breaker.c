// prototype functions in other files and types used for all files.
#include "prototypes.h"

// Making this here so it easily changeable.
// This is used for processing files, could speed things up if increased but will use more memory.
#define BUFFERSIZE 100

// prototype functions only needed in this file.
void printhelp(char option);
int exitcode(int x);
int outputfileheadings(char *output_name, function_struct std_cyphers[], function_struct alt_cyphers[], int numof_std_cyphers, int numof_alt_cyphers, int num_enabled_cyphers, int num_enabled_altcyphers);
int output2file(char *output_name, function_struct std_cyphers[], function_struct alt_cyphers[], int numof_std_cyphers, int numof_alt_cyphers, char *solvedwith, int num_enabled_cyphers, int num_enabled_altcyphers);
int rec_decypher(char *input, int solved_offset, int max_neg_offset, int max_pos_offset, bool required_solved, bool smart_layers, int num_tests, int numof_std_cyphers, int numof_alt_cyphers, char *output_name, int num_enabled_cyphers, int num_enabled_altcyphers);

// Global Variables to be able to clear memory in exit code function.
// f prefix are files
// d prefix is actual data
// o prefix is for options but they are local to the main function.
FILE *f_inputfile = NULL;
FILE *f_alphabetfile = NULL;
FILE *f_solvedfile = NULL;
FILE *f_dictfile = NULL;
FILE *f_outputfile = NULL;
node *linklist = NULL;
char *d_inputfile = NULL;
node *d_alphabet = NULL;
char *d_solvedfile = NULL;
node *d_dictionary[MAX_WORD_LENGTH];
char *d_outputfile = NULL;
time_t time_start = 0;
long long int attempt_counter = 0, attemptmilc = 0, attemptmilp = 0;
int max_layers = 0, current_layer = 0, solvedcount = 0;
cypher_options_struct *cypher_options = NULL;

// This struct used to have function pointers in it but it would require passing through all inputs to every function and still wouldn't be able to iterate through them as they require different prep.
struct function_struct o_cyphers[] = {
    { "vigenere", false},
    { "skip", false},
};
struct function_struct o_alt_cyphers[] = {
    { "reverse", false},
};
struct function_struct o_tests[] = {
    { "ioc", false},
    { "frequency", false}
};

int main(int argc, char *argv[]) {
    // Receive config from running program command
    // Ran command without options
    if (argc == 1) {
        return exitcode(1);
    }
    // Search for help command
    char help[] = "-help";
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i],help) == 0) {
            if ((i + 1) < argc) {
                if (argv[i + 1][0] == 'c') {
                    // printhelp but cyphers variant.
                    printhelp('c');
                    return exitcode(2);
                } else if (argv[i + 1][0] == 't') {
                    // printhelp but tests variant.
                    printhelp('t');
                    return exitcode(2);
                }
            }
            // printhelp function is in this file with all help info.
            printhelp('i');
            return exitcode(2);
        }
    }

    // Setup time
    time_start = time(NULL);

    // Default Options              //
    char *o_inputfile = NULL;       //
    char *o_outputfile = NULL;      //
    char *o_alphabetfile = NULL;    //
    char *o_dictionary = NULL;      //
    char *o_solvedfile = NULL;      //
    int o_solvedoffset = 0;         //
    bool o_requiresolved = false;   // at the moment auto turns on when supplying a solved file. 
    int o_layers = 1;               //
    bool o_smartlayers = false;     // not great implemention at the moment so will default as disabled.


    // Values for the rest of the program
    int num_std_cyphers = sizeof(o_cyphers)/sizeof(o_cyphers[0]);
    int num_alt_cyphers = sizeof(o_alt_cyphers)/sizeof(o_alt_cyphers[0]);
    int num_tests = sizeof(o_tests)/sizeof(o_tests[0]);

// Receive Options, skipping ./breaker
    int optioncount = 0;
    for (int i = 1; i < argc; i++) {
        // Check for key
        if (argv[i][0] == '-') {
            // Checking that the key is only in the format -l,
            if (argv[i][2] != '\0') {
                return exitcode(3);
            }
            // Checking if the key does not have a value
            if (argc == i + 1) {
                return exitcode(3);
            } else if (argv[i+1][0] == '-') {
                return exitcode(3);
            }
            optioncount++;
            // Check and prep values
            // temp variable for values
            int valuecount = 0;
            // loop until next key or end of command
            while (argv[i + valuecount + 1][0] != '-') {
                // Up counter for while block and counter for default values.
                if (argc <= (i + valuecount + 2)) {
                    valuecount++;
                    break;
                } else {
                    valuecount++;
                }
            }
            // Will switch based on what option key was selected and replace with new value/s
            switch (argv[i][1]) {
                case 'i':
                    if (valuecount != 1) {
                        return exitcode(3);
                    }
                    o_inputfile = argv[i+1];
                    break;
                case 'o':
                    if (valuecount != 1) {
                        return exitcode(3);
                    }
                    o_outputfile = argv[i+1];
                    break;
                case 'a':
                    if (valuecount != 1) {
                        return exitcode(3);
                    }
                    o_alphabetfile = argv[i+1];
                    break;
                case 'd':
                    if (valuecount != 1) {
                        return exitcode(3);
                    }
                    o_dictionary = argv[i+1];
                    break;
                case 's': {
                    if (valuecount > 2) {
                        return exitcode(3);
                    }
                    o_requiresolved = true;
                    o_solvedfile = argv[i+1];
                    if (valuecount == 2) {
                        int tempint = 0;
                        for (int j = 0; argv[i+2][j] != '\0'; j++) {
                            if (argv[i+2][j] == '.') {
                                break;
                            } else if (isdigit(argv[i+2][j])) {
                                tempint = tempint * 10 + (argv[i+2][j] - 48);
                            } else {
                                return exitcode(3);
                            }
                        }
                        o_solvedoffset = tempint;
                    }
                    break;
                }
                case 'l': {
                    if (valuecount > 2) {
                        return exitcode(3);
                    }
                    int tempint = 0;
                    for (int j = 0; argv[i+1][j] != '\0'; j++) {
                        if (argv[i+1][j] == '.') {
                            break;
                        } else if (isdigit(argv[i+1][j])) {
                            tempint = tempint * 10 + (argv[i+1][j] - 48);
                        } else {
                            return exitcode(3);
                        }
                    }
                    o_layers = tempint;
                    if (valuecount == 2) {
                        if (strcmp(argv[i+2],"smart") == 0) {
                            o_smartlayers = true;
                        }
                    }
                    break;
                }
                case 'c':
                    for (int j = 0; j < valuecount; j++) {
                        bool found = false;
                        for (int k = 0; k < num_std_cyphers; k++) {
                            if (strcmp(argv[i+j+1],o_cyphers[k].name) == 0) {
                                found = true;
                                o_cyphers[k].enabled = true;
                            }
                        }
                        for (int k = 0; k < num_alt_cyphers; k++) {
                            if (strcmp(argv[i+j+1],o_alt_cyphers[k].name) == 0) {
                                found = true;
                                o_alt_cyphers[k].enabled = true;
                            }
                        }
                        if (found == false) {
                            return exitcode(3);
                        }
                    }
                    break;
                case 't':
                    for (int j = 1; j < valuecount; j++) {
                        bool found = false;
                        for (int k = 0; k < num_tests; k++) {
                            if (strcmp(argv[i+j],o_tests[k].name) == 0) {
                                found = true;
                                o_tests[k].enabled = true;
                            }
                        }
                        if (found == false) {
                            return exitcode(3);
                        }
                    }
                    break;
                default:
                    return exitcode(3);
            }
        } else if (optioncount == 0 && argc > 1) {
            return exitcode(3);
        }
    }

// Process files - doing each manually to add defaults if not selected.
    // Prep variables
    int inputlength = 0, solvedlength = 0;

    // open input files (required)
    if (o_inputfile == NULL) {
        return exitcode(3);
    }
    f_inputfile = fopen(o_inputfile,"r");
    if (f_inputfile == NULL) {
        return exitcode(4);
    }

    // Reading file with linked list and buffer until all is read and then moved into large array.
    // should only be a txt file so shouldn't ever end up being too big for memory. May come back to this in the future if I find an example of this mattering.

    {
        if (linklist != NULL) {
            return exitcode(5);
        }
        int list_counter = 0, string_length = 0;
        char buffer[BUFFERSIZE + 1], letter;
        while (fread(&letter, sizeof(char), 1, f_inputfile)) {
            if (string_length > BUFFERSIZE) {
                buffer[string_length] = '\0';
                int errorcheck = add2list(&linklist, buffer, true);
                if (errorcheck != 0) {
                    return exitcode(errorcheck);
                }
                list_counter++;
                string_length = 0;
            }
            if (isalpha(letter) != 0) {
                buffer[string_length] = letter;
                string_length++;
            }
        }
        // End of file but still chars not loaded into linklist
        if (string_length > 0) {
            buffer[string_length] = '\0';
            int errorcheck = add2list(&linklist, buffer, true);
            if (errorcheck != 0) {
                return exitcode(errorcheck);
            }
        }
        int sizeforinputfile = (list_counter * BUFFERSIZE) + string_length + 1;
        d_inputfile = list2string(&linklist, sizeforinputfile);
        if (d_inputfile == NULL) {
            return exitcode(4);
        }
    }
    fclose(f_inputfile);
    f_inputfile = NULL;

    // open alphabetfile (optional to user) if it fails, load default A-Z.
    if (o_alphabetfile != NULL) {
        f_alphabetfile = fopen(o_alphabetfile,"r");
        if (f_alphabetfile == NULL) {
            return exitcode(4);
        }

        if (linklist != NULL) {
            return exitcode(5);
        }
        int list_counter = 0, string_length = 0;
        char buffer[BUFFERSIZE + 1], letter;
        while (fread(&letter, sizeof(char), 1, f_alphabetfile)) {
            if ((letter == '\n') && ((string_length > 0) || list_counter > 0)) {
                int sizeforalphabet = (list_counter * BUFFERSIZE) + string_length + 1;
                buffer[string_length] = '\0';
                int errorcheck1 = add2list(&linklist, buffer, true);
                if (errorcheck1 != 0) {
                    return exitcode(errorcheck1);
                }
                char *temp_single_alphabet = list2string(&linklist, sizeforalphabet);
                if (temp_single_alphabet == NULL) {
                    return exitcode(4);
                }
                int errorcheck2 = add2list(&d_alphabet,temp_single_alphabet,true);
                if (errorcheck2 != 0) {
                    return exitcode(errorcheck2);
                }
                free(temp_single_alphabet);
                temp_single_alphabet = NULL;
                list_counter = 0;
                string_length = 0;
            } else {
                if (string_length > BUFFERSIZE) {
                    buffer[string_length] = '\0';
                    int errorcheck = add2list(&linklist, buffer, true);
                    if (errorcheck != 0) {
                        return exitcode(errorcheck);
                    }
                    list_counter++;
                    string_length = 0;
                }
                if (isalpha(letter) != 0) {
                    buffer[string_length] = letter;
                    string_length++;
                }
            }
        }
        // End of file but still chars not loaded into linklist
        if (string_length > 0 || list_counter > 0) {
            int sizeforalphabet = (list_counter * BUFFERSIZE) + string_length + 1;
            buffer[string_length] = '\0';
            int errorcheck1 = add2list(&linklist, buffer, true);
            if (errorcheck1 != 0) {
                return exitcode(errorcheck1);
            }
            char *temp_single_alphabet = list2string(&linklist, sizeforalphabet);
            if (temp_single_alphabet == NULL) {
                return exitcode(4);
            }
            int errorcheck2 = add2list(&d_alphabet,temp_single_alphabet,true);
            if (errorcheck2 != 0) {
                return exitcode(errorcheck2);
            }
            free(temp_single_alphabet);
            temp_single_alphabet = NULL;
        }
        fclose(f_alphabetfile);
        f_alphabetfile = NULL;

        // Make alphabet/s uppercase and check for duplicates
        {
            node *templist = d_alphabet;
            while (templist != NULL) {
                int i = 0;
                while (templist->value[i] != '\0') {
                    int j = 0;
                    int count = 0;
                    while (templist->value[j] != '\0') {
                        templist->value[j] = toupper(templist->value[j]);
                        if (templist->value[i] == templist->value[j]) {
                            count++;
                        }
                        j++;
                    }
                    if (count > 1) {
                        return exitcode(8);
                    }
                    i++;
                }
                templist = templist->next;
            }
        }

    } else {
        // makes one node in linked list to operate the same in all cases.
        char *default_alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        int errorcheck = add2list(&d_alphabet,default_alphabet,true);
        if (errorcheck != 0) {
            return exitcode(errorcheck);
        }
    }

    // calculate max alphabet length
    int max_alphabet_length = 0;
    {
        node *templist = d_alphabet;
        while (templist != NULL) {
            if (strlen(templist->value) > max_alphabet_length) {
                max_alphabet_length = strlen(templist->value);
            }
            templist = templist->next;
        }
    }

    // open solvedfile (optional to user in future).
    int max_negative_offset = 0;
    int max_positive_offset = 0;
    if (o_solvedfile != NULL) {
        f_solvedfile = fopen(o_solvedfile,"r");
        if (f_solvedfile == NULL) {
            return exitcode(4);
        }
        {
            if (linklist != NULL) {
                return exitcode(5);
            }
            int list_counter = 0, string_length = 0;
            char buffer[BUFFERSIZE + 1], letter;
            while (fread(&letter, sizeof(char), 1, f_solvedfile)) {
                if (string_length > BUFFERSIZE) {
                    buffer[string_length] = '\0';
                    int errorcheck = add2list(&linklist, buffer, true);
                    if (errorcheck != 0) {
                        return exitcode(errorcheck);
                    }
                    list_counter++;
                    string_length = 0;
                }
                if ((isalnum(letter) != 0) || (letter == '|')) {
                    buffer[string_length] = letter;
                    string_length++;
                }
            }
            // End of file but still chars not loaded into linklist
            if (string_length > 0) {
                buffer[string_length] = '\0';
                int errorcheck = add2list(&linklist, buffer, true);
                if (errorcheck != 0) {
                    return exitcode(errorcheck);
                }
            }
            int sizeforsolvedfile = (list_counter * BUFFERSIZE) + string_length + 1;
            d_solvedfile = list2string(&linklist, sizeforsolvedfile);
            if (d_solvedfile == NULL) {
                return exitcode(4);
            }
        }
        fclose(f_solvedfile);
        f_solvedfile = NULL;

        // Precheck solved file is formatted correctly
        // Process the solved text here. The solved text needs to be in this format: |0|1|h|1|1|l|1|1|o
        // that would translate to knowing the h, first l and o from hello.
        // | to signify start, then int of unknown chars.
        // | to signify next int which will be no. of known chars
        // | to signify start of known chars
        // repeat until end
        // ends after last char known.
        inputlength = strlen(d_inputfile);
        solvedlength = strlen(d_solvedfile);
        int totalinsolved = 0;
        for (int i = 0, state = 0, unknown = 0, known = 0, counter = 0; i < solvedlength; i++){
            // state should be 0-5, should be what part of the custom format it is reading.
            switch (state) {
                case 0:
                    // reset confirm start of file is okay and reset varibles
                    if (d_solvedfile[i] == '|') {
                        state = 1;
                        unknown = 0;
                        known = 0;
                        counter = 0;
                        // break out of switch statement - doesn't break out of if statement.
                        break;
                    } else {
                        return exitcode(6);
                    }
                case 1:
                    // check if digit or end of digits
                    if (isdigit(d_solvedfile[i])) {
                        unknown *= 10;
                        unknown += (d_solvedfile[i] - 48);
                        break;
                    } else if (d_solvedfile[i] == '|') {
                        // if on first run through
                        if (totalinsolved == 0) {
                            max_negative_offset = unknown;
                        }
                        state = 2;
                        break;
                    } else {
                        return exitcode(6);
                    }
                case 2:
                    // check if digit or end of digits
                    if (isdigit(d_solvedfile[i])) {
                        known *= 10;
                        known += (d_solvedfile[i] - 48);
                        break;
                    } else if (d_solvedfile[i] == '|') {
                        state = 3;
                        break;
                    } else {
                        return exitcode(6);
                    }
                case 3: {
                    // check if char is in the alphabet
                    node *templist = d_alphabet;
                    int alphabetno = 0;
                    while (templist != NULL) {
                        int j = 0;
                        bool indict = false;
                        // check all letters in the alphabet
                        while (templist->value[j] != '\0') {
                            if (toupper(d_solvedfile[i]) == toupper(templist->value[j])) {
                                indict = true;
                                break;
                            }
                            j++;
                        }
                        // if not in this char is not in this alphabet, exit program.
                        if (!indict) {
                            printf("Solved file char no. %i compared to alphabet no. %i is the problem. (First alphabet and char will be no. 1 for reference)\n",i+1,alphabetno+1);
                            return exitcode(7);
                        }
                        // move to next alphabet
                        templist = templist->next;
                        alphabetno++;
                    }
                    // continue checking format
                    counter++;
                    if (counter >= known) {
                        state = 0;
                        totalinsolved = totalinsolved + unknown + known;
                        break;
                    }
                }
            }
        }
        if (totalinsolved > inputlength) {
            printf("Error information: text in the input file exceeds text expected from solved file\n");
            return exitcode(6);
        } else {
            max_positive_offset = inputlength - totalinsolved;
        }
    }

    // Precheck if input file has the same characters as the alphabet provided
    for (int i = 0; i < inputlength; i++) {
        // check all alphabets loaded
        node *templist = d_alphabet;
        int alphabetno = 1;
        while (templist != NULL) {
            int j = 0;
            bool indict = false;
            // check all letters in the alphabet
            while (templist->value[j] != '\0') {
                if (toupper(d_inputfile[i]) == toupper(templist->value[j])) {
                    indict = true;
                    break;
                }
                j++;
            }
            // if not in this char is not in this alphabet, exit program.
            if (!indict) {
                printf("Input file char no. %i compared to alphabet no. %i is the problem.",i,alphabetno);
                return exitcode(7);
            }
            // move to next alphabet
            templist = templist->next;
        }
        alphabetno++;
    }

    // Open and process dictionary
    if (o_dictionary != NULL) {
        f_dictfile = fopen(o_dictionary,"r");
        if (f_dictfile == NULL) {
            return exitcode(4);
        }
    } else {
        f_dictfile = fopen("words_alpha.txt","r");
        if (f_dictfile == NULL) {
            printf("Missing default dictionary \"words_alpha.txt\". Please make sure it is in the same directory as the program is run in.\n");
            return exitcode(4);
        }
    }
    {
        if (linklist != NULL) {
            return exitcode(5);
        }

        int count[MAX_WORD_LENGTH];
        for (int i = 0; i < MAX_WORD_LENGTH; i++) {
            d_dictionary[i] = NULL;
            count[i] = 0;
        }

        char letter, word[MAX_WORD_LENGTH + 1];
        int word_length = 0;
        while (fread(&letter, sizeof(char), 1, f_dictfile)) {
            if (isalpha(letter) != 0) {
                if (word_length <= MAX_WORD_LENGTH) {
                    word[word_length] = toupper(letter);
                    word_length++;
                } else {
                    // Word length is too long/ file not formatted correctly.
                    return exitcode(9);
                }
            } else if (letter == '\n') {
                // signifies end of word and onto next one.
                if (word_length > 0) {
                    word[word_length] = '\0';
                    // dictionary is setup in zero index so word_length = 1 is first value in the dictionary
                    int errorcheck = add2list(&d_dictionary[word_length - 1], word, false);
                    if (errorcheck != 0) {
                        return exitcode(errorcheck);
                    }
                    count[word_length - 1]++;
                    word_length = 0;
                }
            }
        }
        // End of file but still have a word check.
        if (word_length > 0) {
            word[word_length] = '\0';
            int errorcheck = add2list(&d_dictionary[word_length - 1], word, false);
            if (errorcheck != 0) {
                return exitcode(errorcheck);
            }
        }
    }
    fclose(f_dictfile);
    f_dictfile = NULL;

    // open output file last to not make empty files before errors show up. (required)
    char *output_name = NULL;
    char *default_output_name = "output.csv";
    if (o_outputfile == NULL) {
        output_name = default_output_name;
    } else {
        output_name = o_outputfile;
    }
    f_outputfile = fopen(output_name, "wx");
    // fopen mode "wx", w being standard writing mode, x being meaning exclusive to check if file exists.
    if (f_outputfile == NULL) {
        printf("Failure to make file. Either file already exists or you have ran out of memory\n");
        return exitcode(3);
    }
    // Now can write to this file when tests come back postitive.
    // Closing here to dynamically add results to persist if crashed
    fclose(f_outputfile);
    f_outputfile = NULL;

    // Cypher Options Processing
    preprocess_cyphers(o_cyphers, num_std_cyphers, o_alt_cyphers, num_alt_cyphers);
    preprocess_cyphers(o_tests, num_tests, o_tests, num_tests);
    // Count enabled cyphers & tests
    int num_enabled_cyphers = count_cyphers(o_cyphers, num_std_cyphers);
    int num_enabled_altcyphers = count_cyphers(o_alt_cyphers, num_alt_cyphers);
    int num_enabled_tests = count_cyphers(o_tests, num_tests);

    // Could add tests option processing here. This would mainly be in the case of if there was multiple tests added here to test for score of the output. Score being if the output makes sense/ chance it is the result.

    printf("Files read, prepared and Options Loaded.\n");
    printf("Time to load in options ");
    if (convert_time_and_print(time(NULL) - time_start) == 1) {
            return exitcode(4);
    }

    // Prep max layers to allow for special cyphers, removed last duplicate result if last layer does nothing. Duplicates are still possible, but as it is a brute force, will be a waste of time to weed out duplicates. Output is a CSV so analysis can be done easily if needed.
    if (num_enabled_altcyphers > 0) {
        max_layers = (o_layers * 2) + 1;
    } else {
        max_layers = (o_layers * 2);
    }

    // Prep options to be able to write to file
    for (int i = 0; i < max_layers; i++){
        cypher_options_struct *new = malloc(sizeof(cypher_options_struct));
        if (new == NULL){
            return exitcode(4);
        }
        new->layer = max_layers - i - 1;
        new->reversed = false;
        new->skips = 0;
        new->keyword = NULL;
        new->cypher_used = NULL;
        new->solution = NULL;
        new->alphabet = NULL;
        new->prev = NULL;
        // add to start of list
        new->next = cypher_options;
        cypher_options = new;
    }
    {
        cypher_options_struct *run1 = cypher_options;
        cypher_options_struct *run2 = cypher_options->next;
        while (run2 != NULL) {
            run2->prev = run1;
            run2 = run2->next;
            run1 = run1->next;
        }
    }

    // Prep Output File
    {
        int errorcheck = outputfileheadings(output_name, o_cyphers, o_alt_cyphers, num_std_cyphers, num_alt_cyphers, num_enabled_cyphers, num_enabled_altcyphers);
        if (errorcheck == 1) {
            return exitcode(10);
        }
    }

// Cypher and tests loop

    // Recursive Function for the actual decyphering

    {
        int errorcheck = rec_decypher(d_inputfile, o_solvedoffset, max_negative_offset, max_positive_offset, o_requiresolved, o_smartlayers, num_tests, num_std_cyphers, num_alt_cyphers, output_name, num_enabled_cyphers, num_enabled_altcyphers);
        switch (errorcheck) {
            case 0:
                // does nothing as the program will need to continue iterating.
                printf("\n");
                break;
            case 4:
                // program ran out of memory.
                return exitcode(4);
            case 10:
                // writing to file has failed.
                return exitcode(10);
            case 99:
                // debug/finish early mode
                printf("Finished Early due to function returning 99\n");
                break;
        }
    }

    // Print how attempt were made
    printf("Attempts made: %lli\n", attempt_counter);
    // Print how many times solved
    printf("Solved Count: %i\n", solvedcount);
    // Print Input and Output File Info
    printf("Input file: %s\nOutput file: %s\n", o_inputfile, output_name);
    // Printing to terminal before exiting program.
    printf("Time Taken Total: ");
    if (convert_time_and_print(time(NULL) - time_start) == 1) {
            return exitcode(4);
    }
    // Clean up and Clear memory
    return exitcode(0);
}

void printhelp(char option) {
    if (option == 'i') {
    // help info for '-help'
        char *helpinfo = "\nThis cypher breaker needs a config to work. You enter the config in the command line when you run the command. This should be familiar if you have used programs like ffmpeg.\n\nOption keys all start with '-' then followed the option value.\nFor example with './breaker -i input.txt', '-i' is the option key, 'input.txt' is the value.\nEvery key needs a value or it won't work. If you don't want to add a value just remove the key and it will run the default value for that key.\n\nPlease read the README.md file included to see all current options.\nThere is also a example folders if you require help, I would recommend running the command for K1 included in the K1 folder and noted in the README.md file.\n\n";
        printf("%s",helpinfo);
    } else if (option == 'c') {
    // help info for '-help c'
        char *cyphershelp = "All available cyphers are as follows:";
        printf("%s\n",cyphershelp);
        int num_std_cyphers = sizeof(o_cyphers)/sizeof(o_cyphers[0]);
        int num_alt_cyphers = sizeof(o_alt_cyphers)/sizeof(o_alt_cyphers[0]);
        for (int i = 0; i < num_std_cyphers; i++) {
            printf("%s\n",o_cyphers[i].name);
        }
        for (int i = 0; i < num_alt_cyphers; i++) {
            printf("%s\n",o_alt_cyphers[i].name);
        }
    } else if (option == 't') {
    // help info for '-help -t'
        char *testshelp = "All available tests are as follows:";
        printf("%s\n",testshelp);
        int num_tests = sizeof(o_tests)/sizeof(o_tests[0]);
        for (int i = 0; i < num_tests; i++) {
            printf("%s\n",o_tests[i].name);
        }
    }
}

int exitcode(int x) {
    // close all files
    if (f_inputfile != NULL) {
        fclose(f_inputfile);
    }
    if (f_alphabetfile != NULL) {
        fclose(f_alphabetfile);
    }
    if (f_solvedfile != NULL) {
        fclose(f_solvedfile);
    }
    if (f_dictfile != NULL) {
        fclose(f_dictfile);
    }
    if (f_outputfile != NULL) {
        fclose(f_outputfile);
    }

    // Clear temp linklist incase of error after loading values
    clearlist(&linklist);

    // clear data
    if (d_inputfile != NULL) {
        free(d_inputfile);
        d_inputfile = NULL;
    }
    clearlist(&d_alphabet);
    if (d_solvedfile != NULL) {
        free(d_solvedfile);
        d_solvedfile = NULL;
    }
    for (int i = 0; i < MAX_WORD_LENGTH; i++) {
        clearlist(&d_dictionary[i]);
    }

    cypher_options = list_traverse(cypher_options, 0);
    if (cypher_options != NULL) {
        cypher_options_struct *templist = cypher_options;
        while (templist != NULL) {
            cypher_options = cypher_options->next;
            if (templist->keyword != NULL) {
                free(templist->keyword);
                templist->keyword = NULL;
            }
            if (templist->cypher_used != NULL) {
                free(templist->cypher_used);
                templist->cypher_used = NULL;
            }
            if (templist->solution != NULL) {
                free(templist->solution);
                templist->solution = NULL;
            }
            if (templist->alphabet != NULL) {
                free(templist->alphabet);
                templist->alphabet = NULL;
            }
            free(templist);
            templist = cypher_options;
        }
    }

    // switch for what error it was.
    switch (x) {
        case 0:
            printf("Program finished correctly.\n");
            return 0;
        case 1:
            printf("Missing options and input file. Run './breaker -help' for more information.\n");
            return 1;
        case 2:
            // printed help command in it's own function.
            return 2;
        case 3:
            printf("Ran program incorrectly, please check the option key and values.\n");
            return 3;
        case 4:
            printf("Failure to open file or Out of Memory.\n");
            return 4;
        case 5:
            // shouldn't be needed but saves me debugging later as I will be reusing this temp linked list for easier freeing.
            printf("This is a Bug Report: linklist was used but it was not cleared in the previous operation.\nStopped program as the rest will not work properly.\n");
            return 5;
        case 6:
            printf("Solved file formatting is not correct.\nPlease refer to documentation or Run './breaker -help' for more information.\n");
            return 6;
        case 7:
            printf("Either input or solved char files have chars not expected in one of the alphabets. If no alphabet was submited, there are chars in the input or solved file that are not A-Z.\n");
            return 7;
        case 8:
            printf("An alphabet submitted has duplicate charaters.\n");
            return 8;
        case 9:
            printf("Dictionary Submitted has words that are longer than the max word length. Either increase max word length at top of breaker.c file before compiling or correct the dictionary formatting. Correct formatting is on the README file.\n");
            return 9;
        case 10:
            printf("Writing to file has failed.\n");
            return 10;
    }
    // incase the error number specified doesn't match one set.
    return 999;
}

int outputfileheadings(char *output_name, function_struct std_cyphers[], function_struct alt_cyphers[], int numof_std_cyphers, int numof_alt_cyphers, int num_enabled_cyphers, int num_enabled_altcyphers) {
    // Open and check file
    f_outputfile = fopen(output_name, "a");
    // fopen mode "a", as appending the file made earlier.
    if (f_outputfile == NULL) {
        return 1;
    }
    // Output in csv format
    // Static headings (headings that will be in every file.)
    char *static_headings = "\"Attempt No.\", \"Solved with\", \"Solution\", \"Layers used\", ";
    {
        int errorcheck = fwrite(static_headings, sizeof(char) * strlen(static_headings),1,f_outputfile);
        if (errorcheck != 1) {
            return 1;
        }
    }
    // Dynamic headings (based on what cyphers are enabled & how many layers was selected)
    char *standard_dynamics = "\"Layer no.\", \"Cypher used\", ";
    // There are better ways to do this, I am more intrested in optimising the rest of the code atm.
    char *string_reversed = "\"Reversed?\", ";
    char *string_keyword = "\"Alphabet\", \"Keyword\", ";
    char *string_skips = "\"Charater Skips\", ";

    for (int i = 0; i < max_layers; i++) {
        // write alt cypher requirements
        if ((i % 2) == 0) {
            if (num_enabled_altcyphers > 0) {
                {
                    int errorcheck = fwrite(standard_dynamics, sizeof(char) * strlen(standard_dynamics),1,f_outputfile);
                    if (errorcheck != 1) {
                        return 1;
                    }
                }
                for (int j = 0; j < numof_alt_cyphers; j++){
                    if ((strcmp(alt_cyphers[j].name,"reverse") == 0) && alt_cyphers[j].enabled) {
                        int errorcheck = fwrite(string_reversed, sizeof(char) * strlen(string_reversed),1,f_outputfile);
                        if (errorcheck != 1) {
                            return 1;
                        }
                    }
                }
            }
        // write standard cypher requirements
        } else {
            if (num_enabled_cyphers > 0) {
                {
                    int errorcheck = fwrite(standard_dynamics, sizeof(char) * strlen(standard_dynamics),1,f_outputfile);
                    if (errorcheck != 1) {
                        return 1;
                    }
                }
                for (int j = 0; j < numof_std_cyphers; j++){
                    if ((strcmp(std_cyphers[j].name,"vigenere") == 0) && std_cyphers[j].enabled) {
                        int errorcheck = fwrite(string_keyword, sizeof(char) * strlen(string_keyword),1,f_outputfile);
                        if (errorcheck != 1) {
                            return 1;
                        }
                    } else if ((strcmp(std_cyphers[j].name,"skip") == 0) && std_cyphers[j].enabled) {
                        int errorcheck = fwrite(string_skips, sizeof(char) * strlen(string_skips),1,f_outputfile);
                        if (errorcheck != 1) {
                            return 1;
                        }
                    }
                }
            }
        }
    }
    // new line in prep for next write to the file
    {
        int errorcheck = fwrite("\n", sizeof(char),1,f_outputfile);
        if (errorcheck != 1) {
            return 1;
        }
    }

    // close file
    fclose(f_outputfile);
    f_outputfile = NULL;
    return 0;
}

int output2file(char *output_name, function_struct std_cyphers[], function_struct alt_cyphers[], int numof_std_cyphers, int numof_alt_cyphers, char *solvedwith, int num_enabled_cyphers, int num_enabled_altcyphers) {
    // Open and check file
    f_outputfile = fopen(output_name, "a");
    // fopen mode "a", as appending the file made earlier.
    if (f_outputfile == NULL) {
        return 1;
    }
    // Output a line of csv.

    // Write standard values, Headings: Attempt No., Solution, Layers used
    {
        int errorcheck = fprintf(f_outputfile,"\"%lli\",\"%s\",\"%s\",\"%i\",",attempt_counter,solvedwith,cypher_options->solution,current_layer);
        if (errorcheck < 0) {
            return 1;
        }
    }

    // Write Dynamic Values
    for (int i = 0; i < current_layer; i++) {

        cypher_options = list_traverse(cypher_options, i);

        // Headings that are dynamic but importantly the exact same order of operations as the headings create formula so that they all match up.
        if ((i % 2) == 0) {
            if (num_enabled_altcyphers > 0) {
                // Headings: Layer no., Cypher used
                {
                    int errorcheck = fprintf(f_outputfile,"\"%i\",\"%s\",",i,cypher_options->cypher_used);
                    if (errorcheck < 0) {
                        return 1;
                    }
                }
                for (int j = 0; j < numof_alt_cyphers; j++){
                    if ((strcmp(alt_cyphers[j].name,"reverse") == 0) && alt_cyphers[j].enabled) {
                        if (strcmp(cypher_options->cypher_used,"reverse") == 0) {
                            int errorcheck = fprintf(f_outputfile,"\"%s\",", cypher_options->reversed ? "true" : "false");
                            if (errorcheck < 0) {
                                return 1;
                            }
                        } else {
                            int errorcheck = fwrite("\"N/A\",",(sizeof(char) * 6),1,f_outputfile);
                            if (errorcheck != 1) {
                                return 1;
                            }
                        }
                    }
                }
            }
        } else {
            if (num_enabled_cyphers > 0) {
                // Headings: Layer no., Cypher used
                {
                    int errorcheck = fprintf(f_outputfile,"\"%i\",\"%s\",",i,cypher_options->cypher_used);
                    if (errorcheck < 0) {
                        return 1;
                    }
                }
                for (int j = 0; j < numof_std_cyphers; j++){
                    if ((strcmp(std_cyphers[j].name,"vigenere") == 0) && std_cyphers[j].enabled) {
                        if (strcmp(cypher_options->cypher_used,"vigenere") == 0) {
                            int errorcheck = fprintf(f_outputfile,"\"%s\",\"%s\",",cypher_options->alphabet,cypher_options->keyword);
                            if (errorcheck < 0) {
                                return 1;
                            }
                        } else {
                            int errorcheck = fwrite("\"N/A\",\"N/A\",",(sizeof(char) * 12),1,f_outputfile);
                            if (errorcheck != 1) {
                                return 1;
                            }
                        }
                    } else if ((strcmp(std_cyphers[j].name,"skip") == 0) && std_cyphers[j].enabled) {
                        if (strcmp(cypher_options->cypher_used,"skip") == 0) {
                            int errorcheck = fprintf(f_outputfile,"\"%i\",",cypher_options->skips);
                            if (errorcheck < 0) {
                                return 1;
                            }
                        } else {
                            int errorcheck = fwrite("\"N/A\",",(sizeof(char) * 6),1,f_outputfile);
                            if (errorcheck != 1) {
                                return 1;
                            }
                        }
                    }
                }
            }
        }
    }

    // new line in prep for next write to the file
    {
        int errorcheck = fwrite("\n", sizeof(char),1,f_outputfile);
        if (errorcheck != 1) {
            return 1;
        }
    }

    // close file
    fclose(f_outputfile);
    f_outputfile = NULL;
    return 0;
}

int rec_decypher(char *input, int solved_offset, int max_neg_offset, int max_pos_offset, bool required_solved, bool smart_layers, int num_tests, int numof_std_cyphers, int numof_alt_cyphers, char *output_name, int num_enabled_cyphers, int num_enabled_altcyphers) {
    attempt_counter++;
    attemptmilc++;
    if (attemptmilc > 999999) {
        attemptmilc = 0;
        attemptmilp++;
        printf("\rSolved Count: %i, Attempt: %lli Million",solvedcount, attemptmilp);
        fflush(stdout);
    }

    // Solved checks on input
    // Check against solved file with offset
    if (d_solvedfile != NULL) {
        int pos_offset = 0;
        int neg_offset = 0;
        if (solved_offset < max_neg_offset) {
            neg_offset = solved_offset;
        } else {
            neg_offset = max_neg_offset;
        }
        if (solved_offset < max_pos_offset) {
            pos_offset = solved_offset;
        } else {
            pos_offset = max_pos_offset;
        }

        for (int i = neg_offset; i < (pos_offset + 1); i++) {
            if (f_matchesSolved(input, d_solvedfile, i) == 1) {
                if (output2file(output_name, o_cyphers, o_alt_cyphers, numof_std_cyphers, numof_alt_cyphers, "Partly Solved File", num_enabled_cyphers, num_enabled_altcyphers) == 1) {
                    return 10;
                }
                solvedcount++;
            }
        }
    }

    // Check against readability tests (probably won't be done for the submittion)
    if ((d_solvedfile == NULL) || (required_solved == false)) {
        // If solved file is not sumbitted or not required, loop through readability tests to check score of readability.
            // If readability test is enabled (can be multiple), test for readability
                // If meets user score of readability, write to output file in custom format to detail all info regarding how we got to that result.
    }

    // End Recursion Check
    if (max_layers <= current_layer) {
        current_layer--;
        return 0;
    } else {
    // Decypher Part

        // Makes sure cypher_options is on the correct layer
        cypher_options = list_traverse(cypher_options, current_layer);

        // This ignores the alt layer and adjusts for the zero index.
        int last_layer = max_layers - 2;
        // Odd layers are normal cyphers
        if (((current_layer % 2) == 1) && (num_enabled_cyphers > 0)) {
            for (int i = 0; i < numof_std_cyphers; i++) {
                if (o_cyphers[i].enabled) {

                    if (cypher_options->cypher_used != NULL) {
                        free(cypher_options->cypher_used);
                        cypher_options->cypher_used = NULL;
                    }
                    cypher_options->cypher_used = malloc(sizeof(char) * (strlen(o_cyphers[i].name) + 1));
                    if (cypher_options->cypher_used == NULL) {
                        return 4;
                    }
                    strcpy(cypher_options->cypher_used,o_cyphers[i].name);

                    if (strcmp(o_cyphers[i].name,"vigenere") == 0) {
                        // loop through alphabets here as they are applicable to the vigenere cypher
                        node *alphabet_copy = d_alphabet;
                        while (alphabet_copy != NULL) {

                            if (cypher_options->alphabet != NULL) {
                                free(cypher_options->alphabet);
                                cypher_options->alphabet = NULL;
                            }
                            cypher_options->alphabet = malloc(sizeof(char) * (strlen(alphabet_copy->value) + 1));
                            if (cypher_options->alphabet == NULL) {
                                return 4;
                            }
                            strcpy(cypher_options->alphabet,alphabet_copy->value);

                            // There is gonna be no smart stuff for vigenere for now, will need significant work and testing for algorithms to reduce keylength iterations.
                            // Until then, forcing using a dictionary of english words which isn't ideal.

                            for (int keylength = 0; keylength < MAX_WORD_LENGTH; keylength++) {
                                node *dict_list = d_dictionary[keylength];
                                while (dict_list != NULL) {

                                    if (cypher_options->keyword != NULL) {
                                        free(cypher_options->keyword);
                                        cypher_options->keyword = NULL;
                                    }
                                    cypher_options->keyword = malloc(sizeof(char) * (strlen(dict_list->value) + 1));
                                    if (cypher_options->keyword == NULL) {
                                        return 4;
                                    }
                                    strcpy(cypher_options->keyword,dict_list->value);

                                    if (cypher_options->solution != NULL) {
                                        free(cypher_options->solution);
                                        cypher_options->solution = NULL;
                                    }

                                    cypher_options->solution = f_vigenere(input, alphabet_copy->value, dict_list->value);
                                    if (cypher_options->solution == NULL) {
                                        return 4;
                                    }

                                    current_layer++;

                                    {
                                        int errorcheck = rec_decypher(cypher_options->solution, solved_offset, max_neg_offset, max_pos_offset, required_solved, smart_layers, num_tests, numof_std_cyphers, numof_alt_cyphers, output_name, num_enabled_cyphers, num_enabled_altcyphers);
                                        switch (errorcheck) {
                                            case 0:
                                                // does nothing as the program will need to continue iterating.
                                                break;
                                            case 4:
                                                // program ran out of memory.
                                                return 4;
                                            case 10:
                                                // writing to file has failed.
                                                return 10;
                                            case 99:
                                                // debug/finish early mode
                                                return 99;
                                        }
                                        cypher_options = list_traverse(cypher_options, current_layer);
                                    }
                                    dict_list = dict_list->next;
                                }
                            }
                            alphabet_copy = alphabet_copy->next;
                        }
                    } else if (strcmp(o_cyphers[i].name,"skip") == 0) {
                        bool continue_after_tests = true;
                        if ((current_layer >= last_layer) && (smart_layers)) {
                            continue_after_tests = false;
                            for (int j = 0; j < num_tests; j++) {
                                // Frequency test is only used to test if it is the english language, not used to predict letters.
                                if (o_tests[i].enabled == true) {
                                    int test_output = 0;
                                    if (strcmp(o_tests[i].name,"frequency") == 0) {
                                        test_output = f_frequency(input);
                                        if (test_output == 1) {
                                            continue_after_tests = true;
                                        } else if (test_output == 2) {
                                            printf("Minor Error (Program will continue): input length is too short to check frequency.");
                                            continue_after_tests = true;
                                        }
                                    }
                                    if (strcmp(o_tests[i].name,"ioc") == 0) {
                                        test_output = f_index_of_coincidence(input, false);
                                        if (test_output == 1) {
                                            continue_after_tests = true;
                                        } else if (test_output == 2){
                                            printf("Minor Error (Program will continue): f_index_of_coincidence is broken, will treat it as if it passed.\n");
                                            continue_after_tests = true;
                                        }
                                    }
                                    // Add more tests here
                                }
                            }
                        }
                        if (continue_after_tests) {
                            int inputlength = strlen(input);
                            // skip function can not have an input length of 0 or accesses non-allocated memory.
                            for (int j = 1; j < inputlength; j++) {
                                if (cypher_options->solution != NULL) {
                                    free(cypher_options->solution);
                                    cypher_options->solution = NULL;
                                }
                                cypher_options->skips = j;
                                cypher_options->solution = f_skip(input,j);
                                if (cypher_options->solution == NULL) {
                                    return 4;
                                    // 4 means ran out of memory in this program. can send it all the way back and in main function "exitcode(4)"
                                }
                                current_layer++;
                                {
                                    int errorcheck = rec_decypher(cypher_options->solution, solved_offset, max_neg_offset, max_pos_offset, required_solved, smart_layers, num_tests, numof_std_cyphers, numof_alt_cyphers, output_name, num_enabled_cyphers, num_enabled_altcyphers);
                                    switch (errorcheck) {
                                        case 0:
                                            // does nothing as the program will need to continue iterating.
                                            break;
                                        case 4:
                                            // program ran out of memory.
                                            return 4;
                                        case 10:
                                            // writing to file has failed.
                                            return 10;
                                        case 99:
                                            // debug/finish early mode
                                            return 99;
                                    }
                                    cypher_options = list_traverse(cypher_options, current_layer);
                                }
                            }
                        }
                    }
                    // if there are more cyphers, they need to be added here.
                }
            }
        // Even layers are alt cyphers (generally a lot shorter processing time)
        } else if (((current_layer % 2) == 0) && (num_enabled_altcyphers > 0)) {
            for (int i = 0; i < numof_alt_cyphers; i++) {
                if (o_alt_cyphers[i].enabled) {
                    if (strcmp(o_alt_cyphers[i].name,"reverse") == 0) {

                        if (cypher_options->cypher_used != NULL) {
                            free(cypher_options->cypher_used);
                            cypher_options->cypher_used = NULL;
                        }
                        cypher_options->cypher_used = malloc(sizeof(char) * (strlen(o_alt_cyphers[i].name) + 1));
                        if (cypher_options->cypher_used == NULL) {
                            return 4;
                        }
                        strcpy(cypher_options->cypher_used,o_alt_cyphers[i].name);

                        for (int j = 0; j < 2; j++) {
                            if (cypher_options->solution != NULL) {
                                free(cypher_options->solution);
                                cypher_options->solution = NULL;
                            }
                            if (j == 1) {
                                cypher_options->reversed = true;
                                cypher_options->solution = f_reverse(input);
                                if (cypher_options->solution == NULL) {
                                    return 4;
                                    // 4 means ran out of memory in this program. can send it all the way back and in main function "exitcode(4)"
                                }
                            } else {
                                cypher_options->reversed = false;
                                cypher_options->solution = malloc(sizeof(char) * (strlen(input) + 1));
                                if (cypher_options->solution == NULL) {
                                    return 4;
                                    // 4 means ran out of memory in this program. can send it all the way back and in main function "exitcode(4)"
                                }
                                strcpy(cypher_options->solution,input);
                            }
                            current_layer++;
                            {
                                int errorcheck = rec_decypher(cypher_options->solution, solved_offset, max_neg_offset, max_pos_offset, required_solved, smart_layers, num_tests, numof_std_cyphers, numof_alt_cyphers, output_name, num_enabled_cyphers, num_enabled_altcyphers);
                                switch (errorcheck) {
                                    case 0:
                                        // does nothing as the program will need to continue iterating.
                                        break;
                                    case 4:
                                        // program ran out of memory.
                                        return 4;
                                    case 10:
                                        // writing to file has failed.
                                        return 10;
                                    case 99:
                                        // debug/finish early mode
                                        return 99;
                                }
                                cypher_options = list_traverse(cypher_options, current_layer);
                            }
                        }
                    }
                    // if there are more special cyphers, they need to be added here
                }
            }
        } else {
            // pass through to next layer if one whole set of cyphers have no enabled cyphers in it, like if you disable reversed
            if (cypher_options->solution != NULL) {
                free(cypher_options->solution);
                cypher_options->solution = NULL;
            }
            cypher_options->solution = malloc(sizeof(char) * (strlen(input) + 1));
            if (cypher_options->solution == NULL) {
                return 4;
                // 4 means ran out of memory in this program. can send it all the way back and in main function "exitcode(4)"
            }
            strcpy(cypher_options->solution,input);
            current_layer++;
            {
                int errorcheck = rec_decypher(cypher_options->solution, solved_offset, max_neg_offset, max_pos_offset, required_solved, smart_layers, num_tests, numof_std_cyphers, numof_alt_cyphers, output_name, num_enabled_cyphers, num_enabled_altcyphers);
                switch (errorcheck) {
                    case 0:
                        // does nothing as the program will need to continue iterating.
                        break;
                    case 4:
                        // program ran out of memory.
                        return 4;
                    case 10:
                        // writing to file has failed.
                        return 10;
                    case 99:
                        // debug/finish early mode
                        return 99;
                }
                cypher_options = list_traverse(cypher_options, current_layer);
            }
        }
    }
    current_layer--;
    return 0;
}
