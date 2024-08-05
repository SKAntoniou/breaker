// for structs and bool
#include "prototypes.h"

struct ordering_IoC {
    int keylength;
    double IoC;
    double variance;
    double from_english;
    bool passed;
};

double f_IoC(char *input, int keylength, int inputlength, int offset, char *alphabetletters, int alphabetlength) {
    // Prep
    alphabet_frequency_struct alphabet_frequency[alphabetlength];
    for (int i = 0; i < alphabetlength; i++) {
        alphabet_frequency[i].letter = alphabetletters[i];
        alphabet_frequency[i].count = 0;
    }
    // Check groups loop
    int cursor = offset;
    int charcount = 0;
    while (cursor < inputlength) {
        for (int j = 0; j < alphabetlength; j++) {
            if (toupper(input[cursor]) == alphabet_frequency[j].letter) {
                alphabet_frequency[j].count++;
                charcount++;
                break;
            }
        }
        cursor += keylength;
    }
    // Calculate IoC
    double IoC = 0;
    for (int i = 0; i < alphabetlength; i++) {
        double tempd = (double) (alphabet_frequency[i].count * (alphabet_frequency[i].count - 1));
        IoC += tempd;
    }
    IoC = IoC / (charcount * (charcount - 1));
    return IoC;
}

int comp_for_qsort(const void *p1, const void *p2) {
    double output = ((struct ordering_IoC*)p1)->from_english - ((struct ordering_IoC*)p2)->from_english;
    if (output < 0) {
        return -1;
    } else if (output == 0) {
        return 0;
    } else {
        return 1;
    }
}


int f_index_of_coincidence(char *input, bool keylength_check) {
    /*
    Constant Output:
    2 = Function failed in someway
    Output that differs based on if keylength_check is true:
    If heylength_check is true:
        this is unfinished as output will likely need to be fed into another algorithm to actually have an inpact on iterations.
    0 =
    1 =
    If false:
        this is if it in the english alphabet
    0 = Failed
    1 = Passed
    */

    // Pass Percentage variable
    int pass_percentage = 75;
    double english_IoC = 0.066538;
    double pass_variance = english_IoC - ((pass_percentage/100) * english_IoC);

    int inputlength = strlen(input);
    char *alphabetletters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int alphabetlength = strlen(alphabetletters);


    if (keylength_check) {
        // most likely move this to an option later
        int MAX_keylength = 30;

        // Prep
        double keylength_array[MAX_keylength][MAX_keylength + 2];
        for (int i = 0; i < MAX_keylength; i++) {
            for (int j = 0; j < MAX_keylength + 2; j++) {
                keylength_array[i][j] = 0.0;
            }
        }

        // keylengths and offset loop
        // Also processing average and stuff
        for (int keylength = 1; keylength < MAX_keylength + 1; keylength++) {
            // offset for varience/accuracy
            for (int offset = 0; offset < keylength; offset++) {
                // IoC for this set
                keylength_array[keylength - 1][offset] = f_IoC(input, keylength, inputlength, offset, alphabetletters, alphabetlength);
                // Average Value
                keylength_array[keylength - 1][MAX_keylength] += keylength_array[keylength - 1][offset];
            }
            // Average Value
            keylength_array[keylength - 1][MAX_keylength] = keylength_array[keylength - 1][MAX_keylength] / keylength;
        }

        // Max Variance Processing - may change to actual variance as this is too forgiving/ inaccurate
        for (int keylength = 1; keylength < MAX_keylength + 1; keylength++) {
            for (int offset = 0; offset < keylength; offset++) {
                // Max variance
                double temp = keylength_array[keylength - 1][MAX_keylength] - keylength_array[keylength - 1][offset];
                if (temp < 0) {
                    temp *= (-1);
                }
                if (temp > keylength_array[keylength - 1][MAX_keylength + 1]) {
                    keylength_array[keylength - 1][MAX_keylength + 1] = temp;
                }
            }
            // printf("Keylength = %i, Average IoC = %f, Variance = %f\n", keylength, keylength_array[keylength - 1][MAX_keylength], keylength_array[keylength - 1][MAX_keylength + 1]);
        }

        // Prep Order Processing
        struct ordering_IoC struct_keylength[MAX_keylength];
        for (int i = 0; i < MAX_keylength; i++) {
            struct_keylength[i].keylength = i + 1;
            struct_keylength[i].IoC = keylength_array[i][MAX_keylength];
            struct_keylength[i].variance = keylength_array[i][MAX_keylength + 1];
            struct_keylength[i].from_english = 0.0;
            struct_keylength[i].passed = false;
            // if english alphabet
            if (((english_IoC - pass_variance) < (struct_keylength[i].IoC + struct_keylength[i].variance)) && ((struct_keylength[i].IoC - struct_keylength[i].variance) < (english_IoC + pass_variance))) {
                struct_keylength[i].passed = true;
            }
        }

        // Order Processing by closest to english IoC
        for (int i = 0; i < MAX_keylength; i++) {
            struct_keylength[i].from_english = struct_keylength[i].IoC - english_IoC;
            if (struct_keylength[i].from_english < 0) {
                struct_keylength[i].from_english *= (-1);
            }
        }
        qsort(struct_keylength, MAX_keylength, sizeof(struct ordering_IoC), comp_for_qsort);

        // Output
        /*
        Not finished as more 'smart' stuff needs implementing to make this useful. Currently forcing using a dictionary of words to reduce iterations. Would need some algorithm to guess the keyword from keylength efficiently.

        Potentially int pointer passed through into this function. Then calculate and malloc here for keylengths.

        Best case would be output top three results but this does not guarantee that it would work so not using this part of the function until more is done. 
        */

    } else {
        double output = f_IoC(input, 1, inputlength, 0, alphabetletters, alphabetlength);
        // printf("IoC = %f\n",output);

        if (((english_IoC - pass_variance) < output) && (output < (english_IoC + pass_variance))) {
            return 1;
        } else {
            return 0;
        }
    }
    return 2;
}

int f_frequency(char *input) {
// Frequency test is only used to test if it is the english language, not used to predict letters.
// 0 = Does not match frequency
// 1 = Matches alphabet
// 2 = input length is too short
    int inputlength = strlen(input);
    int required_inputlength = 0;
    if (inputlength < required_inputlength) {
        return 2;
    }
    // The order of the alphabet doesn't matter for this frequency test.
    char *alphabetletters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int alphabetlength = strlen(alphabetletters);
    alphabet_frequency_struct alphabet_frequency[alphabetlength];
    alphabet_frequency_struct alphabet_frequency_reference[alphabetlength];
    // using wikipedia https://en.wikipedia.org/wiki/Letter_frequency for english alphabet in TEXTS as seems more realistic. The letter frequency may change between time periods so hard to get an ideal frequency so the errors wikipedia will be cancelled out/insignificant compared to the theoretical ideal frequency. If we knew what time period the plaintext was writen, could be more accurate but the whole thing is pointless if we knew that.
    // the reference values are out of 100,000 so 100,000 is 100%, 3,000 is 3% etc...
    int reference_total = 100000;
    int english_AZ_reference_values[] = {8167,1492,2782,4253,12702,2228,2015,6094,6966,253,1772,4025,2406,6749,7507,1929,95,5987,6327,9056,2758,978,2360,250,1974,74};
    for (int i = 0; i < alphabetlength; i++) {
        alphabet_frequency_reference[i].letter = alphabetletters[i];
        alphabet_frequency_reference[i].count = english_AZ_reference_values[i];
        alphabet_frequency[i].letter = alphabetletters[i];
        alphabet_frequency[i].count = 0;
    }
    for (int i = 0; i < inputlength; i++) {
        // reduce input length if not A-Z as will mess with future logic
        if (isalpha(input[i])) {
            for (int j = 0; j < alphabetlength; j++) {
                if (toupper(input[i]) == alphabet_frequency[j].letter) {
                    alphabet_frequency[j].count++;
                    break;
                }
            }
        } else {
            inputlength--;
        }
    }
    double multiplier = reference_total/inputlength;
    for (int i = 0; i < alphabetlength; i++) {
        alphabet_frequency[i].count = alphabet_frequency[i].count * multiplier;
    }
    // logic to compare the values of the frequency test
    int required_chars = 23;
    int forgiveness_chars = 2;
    // Actual logic
    int deviation = forgiveness_chars * (1000/inputlength);
    int correct_counter = 0;
    for (int i = 0; i < alphabetlength; i++) {
        int upper_dev = alphabet_frequency_reference[i].count + deviation;
        int lower_dev = alphabet_frequency_reference[i].count - deviation;
        if ((lower_dev < alphabet_frequency[i].count) && (alphabet_frequency[i].count < upper_dev)) {
            correct_counter++;
        }
    }
    if (correct_counter >= required_chars) {
        return 1;
    }
    return 0;
}


int f_matchesSolved(char *input, char *solved, int offset){
// test against solved text.
// type is int to be able to handle errors as well.
// 0 = Does not match solved text
// 1 = matches solved text
// 6 = error from formatting but shouldn't be possible as the solved file should have been prechecked
    // Process the solved text here. The solved text needs to be in this format: |0|1|h|1|1|l|1|1|o
    // that would translate to knowing the h, first l and o from hello.
    // | to signify start, then int of unknown chars.
    // | to signify next int which will be no. of known chars
    // | to signify start of known chars
    // repeat until end
    // ends after last char known.

    int inputlength = strlen(input);
    int solvedlength = strlen(solved);

    for (int i = 0, state = 0, unknown = 0, known = 0, counter = 0, total = 0; i < solvedlength; i++){
        // state should be 0-5, should be what part of the custom format it is reading.
        switch (state) {
            case 0:
                // reset confirm start of file is okay and reset varibles
                if (solved[i] == '|') {
                    state = 1;
                    unknown = 0;
                    known = 0;
                    counter = 0;
                    // break out of switch statement - doesn't break out of if statement.
                    break;
                } else {
                    return 6;
                }
            case 1:
                // check if digit or end of digits
                if (isdigit(solved[i])) {
                    unknown *= 10;
                    unknown += (solved[i] - 48);
                    break;
                } else if (solved[i] == '|') {
                    state = 2;
                    break;
                } else {
                    return 6;
                }
            case 2:
                // check if digit or end of digits
                if (isdigit(solved[i])) {
                    known *= 10;
                    known += (solved[i] - 48);
                    break;
                } else if (solved[i] == '|') {
                    state = 3;
                    break;
                } else {
                    return 6;
                }
            case 3:
                // check against solved text
                if (toupper(input[total+unknown+offset+counter]) !=  toupper(solved[i])) {
                    return 0;
                }
                counter++;
                if (counter >= known) {
                    state = 0;
                    total = total + unknown + known;
                    break;
                }
        }
    }
    // matches solved text
    return 1;
}


