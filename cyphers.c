// for structs and bool
#include "prototypes.h"

char *f_reverse(char *input){
    int inputlength = strlen(input);
    char *output = malloc(sizeof(char) * (inputlength + 1));
    if (output == NULL){
        return NULL;
    }
    for (int i = 0; i < inputlength; i++) {
        output[i] = input[inputlength - i - 1];
    }
    output[inputlength] = '\0';
    return output;
}

char *f_skip(char *input, int skips){
    int inputlength = strlen(input);
    if ((skips < 1) || (skips >= inputlength)) {
       return NULL;
    }
    char *output = malloc(sizeof(char) * (inputlength + 1));
    if (output == NULL){
        return NULL;
    }
    int cursor = 0;
    for (int i = 0; i < inputlength; i++) {
        cursor += skips;
        while (cursor > inputlength) {
            cursor -= inputlength;
        }
        output[i] = input[cursor-1];
    }
    output[inputlength] = '\0';
    return output;
}

char *f_vigenere(char *input, char *alphabet, char *keyword){
    int inputlength = strlen(input);
    char *output = malloc(sizeof(char) * (inputlength + 1));
    if (output == NULL){
        return NULL;
    }
    int alphabetlength = strlen(alphabet);
    int keywordlength = strlen(keyword);
    // convert keyword into an array of int
    int keywordint[keywordlength];
    char keyword_letter;
    for (int i = 0; i < keywordlength; i++) {
        keyword_letter = toupper(keyword[i]);
        for (int j = 0; j < alphabetlength; j++){
            if (keyword_letter == alphabet[j]) {
                keywordint[i] = j;
                break;
            }
        }
    }
    // convert input into an array of int and calculate for output
    char input_letter;
    for (int i = 0; i < inputlength; i++) {
        input_letter = toupper(input[i]);
        for (int j = 0; j < alphabetlength; j++){
            if (input_letter == alphabet[j]) {
                int tempint = j - keywordint[i % keywordlength];
                if (tempint < 0) {
                    tempint += alphabetlength;
                }
                output[i] = alphabet[tempint];
                break;
            }
        }
    }
    output[inputlength] = '\0';
    return output;
}
