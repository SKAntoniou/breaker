# Brute Force Cryptography Breaker

#### Video Demo: 
#### Description:

Inspired by KRYPTOS at CIA HQ, brute force crytography cracker with options that resemble FFMPEG usage. Currently only supports outputing when you have a partial solution you can feed into it. Designed specifically for cypher text that has been encrypted mutliple times.

#### Credits

Dictionary from [github user dwyl](https://github.com/dwyl/english-words)
Only used words_alpha.txt as it would be used for keyword in vignere cypher. This is preformatted in the correct way so no changes need to be made.
Duck Debugger from CS50x Cource was the only AI used and was used for no code production and very sparingly. This is my first fully solo coding project so having it as a sanitiy check was useful when it worked.

Research sites used and useful if you want to read into cryptography or Kryptos more.
LEMMiNO's Kryptos [YouTube video](https://www.youtube.com/watch?v=jVpsLMCIB0Y). Great overview on the Kryptos project.
dCode [cipher website](https://www.dcode.fr/) for load of just cypher information and calculators.
[The Kryptos Project](https://www.thekryptosproject.com) website. This a bunch of information on how the previous sections were solved.

## Usage

This program is CPU dependent, it uses very little RAM and currently is only single threaded. Multithreading is a priority to make performance better.

### Compiling

To compile the program, files breaker.c, prototypes.h, cyphers.c, helpers.c, testers.c & Makefile need to be in the folder and just run command below.

```terminal
make -B breaker
```

### Files

All input files will be txt files and formatted as below.
Output will be a CSV file. You can change the name (through options) but not the formatting. Default is output.csv.

#### Cypher Text

Only cypher text, new lines and spaces will be ignored. If i find an example of spaces being nessassary, may improve the program but for this usecase, it works.

#### Alphabet

Supports multiple alphabets and will iterate through them. Will need to be formatted as one full alphabet per line, ideally uppercase only. Will default to ABC....XYZ if no option is submitted.

#### Dictionary

A dictionary is required. Default is words_alpha.txt, this needs to be in the same directory as the program. Default can be changed with input options.

#### Partially Solved

Custom Formatting and examples have been included in folders. Custom formatting algorithm below.

| to signify start, then number of unknown chars.
| to signify next number which will be no. of known chars
| to signify start of known chars
Write out known chars
Repeat until end
Ends after last known char

Example 1, ignoring cypher text:
Plain Text:
THISTEXTISGOINGTOSAYHELLOWORLD
Known Text:
??????????GOING?????HELLO?????
Formatted for file:
|10|5|GOING|5|5|HELLO

Example 2:
Plain Text:
HELLOWORLD
Known Text:
HELLO?????
Formatted for file:
|0|5|HELLO

### Options

Running the program, the rest of the options follow this one.

```terminal
./breaker
```

#### Help Commands

```terminal
-help
```

Show supported cyphers

```terminal
-help c
```

Show supported tests

```terminal
-help t
```

#### Files Options

Cypher Text File

```terminal
-i input.txt
```

Partially solved text with an optional offset in case the partial solved unconfirmed where it lies. The order is important. Only one of the two below is required, is two are included, only one will be processed and may cause issues.

```terminal
-s solved.txt
-s solved.txt 1
```

Alphabet File (required for only cyphers that have been encrypted with vigenere and custom alphabet)
If unsure if required, this supports multiple alphabets and will iterate through them when needed.
Formatting is just one alphabet per line, no duplicate letters per line.

```terminal
-a alphabet.txt
```

Dictionary File, will default to 'words_alpha.txt' if nothing is supplied.

```terminal
-d dictionary.txt
```

Output File, will default to output.csv if nothing is supplied. File cannot exist before starting program.

```terminal
-o output.csv
```

#### Running Options

Layers and optional 'smart' option.
Layers will state how my cyphers one cypher text has been encrypted in. So K1, K2 & K3 Kryptos are all 1 layer as each only used one cypher to encrypt the plaintext into the cypher text. Layers will be need to be a number but can be any number. Would suggest only 1 or 2.
'Smart' option is for tests that will reduce the iterations needed. This is by default off and is not optimised at the moment so I suggest keeping it off. If turned on, this will only be operational on the last layer.
Only one of the two command below should be run.
For reference if everything is enabled with no smart option left disabled, the attempt made will be estimated below:
Layers = 1, Attempts ~ 2.2 Million
Layers = 2, Attempts ~ 5,000,000 Million
Safe to say anything above two layers would require multitreading, a high performance CPU and a lot of time.

```terminal
-l 2
-l 2 smart
```

Specify Cyphers Used. Would recommend running './breaker -help c' first to see what all the cyphers are called. This will disable cyphers not stated. Below are example but it can be any combination of any or all. This is case sensitive so run the help command if unsure on the spelling.
By default all cyphers will be enabled, this is just if you want to reduce iterations.

```terminal
-c vigenere
-c vigenere skip
-c skip reverse
```

Specify Tests Used. Would recommend running './breaker -help t' first to see what all the test are called. This won't affect the if solved test, that is seperate. Tests also require smart layers being active which is off by default.

```terminal
-t ioc
-t frequency
```

### Current Limitations

1. Case Sensitive input / output. For the current cyphers, shouldn't matter but would be a minor problem if needing that functionality.

2. Option key being more than one char. This can be fixed but not needed for current size of program.

3. Brute Force alphabet. Not implemeted as would take too long to iterate through.

4. Needs to have a partial solution. Easy to expand to add more tests to remove this limitation and may add an english test in the future.

5. No multithreading at the moment, may implement shortly as should signicantly improve performance.

6. Limited options for test pass marks, currently you need to edit the testers.c file with the changes you want to make. May also change this in future to add more options in CLI when running the command.

7. One output file, and may break if user edits it before program is finished. May make CSV files dynamically then merge them at the end to be safer, but not implemented yet.

8. Input text/file cannot contain symbols, this was made this way as more of a precaution. Could be changed to add a random letter where there was a symbol but I don't require this functionality. I have still included K2 & K3 folders with all the formatted files needed if you would like to improve / test your implementation of this.

9. Not really a limitation but variable and function names are a bit of a mess, I didn't have naming convention planned out when starting. Retroactively fixed some but still a bit of a mess.

### Exit Status Codes

If program fails to finish, it should print an error message. If it doesn't use command below to find the exitcode

```terminal
echo $?
```

Reference Table

| Error No. | Description                                                                                  |
| --------- | -------------------------------------------------------------------------------------------- |
| 0         | Program finished correctly                                                                   |
| 1         | Missing Required Options or Options were not correct                                         |
| 2         | The program finished correctly, but the help menu was called                                 |
| 3         | Options were not correct                                                                     |
| 4         | Ran out of Memory                                                                            |
| 5         | The program did not clear the reused linked list. This is just a bug report.                 |
| 6         | Solved file formatting was not correct                                                       |
| 7         | Input or Solved have extra chars not aligning with the alphabet                              |
| 8         | Alphabet has duplicate chars                                                                 |
| 9         | Dictionary submitted is not formatted correctly/ a word is longer than the allowed max of 45 |
| 10        | Writing to file has failed.                                                                  |
| 999       | The exit function has been misused or is broken.                                             |

### Cyphers Included

Only support two main cyphers at the moment as they have been used for the other parts of Kryptos.

#### Vigenere

Used for Kryptos K1 & K2.
Requires alphabet and keyword.

#### Rotation Transformation

Used for Kryptos K3.
Requires Rotation data and size of block to rotate around. This can just be calculated though by multipling the row dimensions against each other for every rotation to get a 'skips' value. This is far simpler and means less iteration. Just need to iterate through skip value which would be at maximum the length of the input text.

#### Reversed

Iterates between reversed and not inbetween cypher attempts

### Tests Included

#### Index of Coincidence

Split up into many parts, this can be used to guess if text is in a certain language or used to check keylengths for cyphers such as vignere. It is setup to check keylengths but not used as would need at least another algorithm to make it useful. This will be useful to make the dictionary not needed.

#### Frequency Analysis

Provides the same purpose as the Index of Coincidence except only setup to check if english alphabet atm, not keylengths.

#### Solved function

Used to check if the output of a decypher attempt matches the partially solved text.

## Full Examples

### K1 Kryptos

For this example, we will pretend we know part of the plain text. There is a folder included called K1, this has all the files needed to test this. We will run it with as many default settings to make it more realistic.

We would have had to work out the alphabet would not be in the same order / a vigenere was to be used. Also we would need to know part of the plain text. These are just the limitations of the program atm. Brute forcing an alphabet wouldn't be too difficult but would take far too long to run so was not implemented.

So commands below first compile the application (if you are in the correct directory) and then run it.

```terminal
make -B breaker

./breaker -i K1/K1_input.txt -a K1/K_alphabets.txt -s K1/K1_part_solved.txt
```

You should get the default output file "output.csv" in your directory.

If you have duplicate solutions, this is because it will have tested for not reversed the text, which is used when layers are more than one.

### K2 & K3 Kryptos

These currently do not work due to current limitation no. 8. I have still included the folder if you would like the test it out. To force it to work anyway, you would need to change any symbols in the input file to random chars and then change them back after finished. 

### K4 Kryptos

I have included K4 Kryptos as it was the inspiration for this project. Feel free to mess with options / add cyphers etc to see if you can crack it.
