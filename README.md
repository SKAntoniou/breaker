# Brute Force Cryptography Breaker

#### Description

Inspired by KRYPTOS at CIA HQ, this brute-force cryptography cracker has options that resemble FFMPEG usage. It currently only supports outputting when you have a partial solution you can feed into it. It was explicitly designed for cypher text that has been encrypted multiple times.

#### Credits

Dictionary from [github user dwyl](https://github.com/dwyl/english-words)
Only used words_alpha.txt as it would be used for keywords in vignette cypher. This is preformatted correctly, so no changes need to be made.
Duck Debugger from CS50x Cource was the only AI used. It was used sparingly and for no code production. This is my first entirely solo coding project, so having it as a checker was helpful when it worked.

The research sites I used are helpful if you want to read more about cryptography or Kryptos.
LEMMiNO's Kryptos [YouTube video](https://www.youtube.com/watch?v=jVpsLMCIB0Y). Excellent of the Kryptos project.
dCode [cypher website](https://www.dcode.fr/) for just cypher information and calculators.
[The Kryptos Project](https://www.thekryptosproject.com) website. This is a bunch of information on how the previous sections were solved.

## Usage

This program is CPU-dependent, uses minimal RAM and is currently only single-threaded. To improve performance, multithreading is a priority.

### Compiling

To compile the program, files breaker.c, prototypes.h, cyphers.c, helpers.c, testers.c & Makefile need to be in the folder. Just run the command below.

```terminal
make -B breaker
```

### Files

All input files will be txt files and formatted as below.
The output will be a CSV file. You can change the name (through options) but not the formatting. The default is output.csv.

#### Cypher Text

Only cypher text, new lines, and spaces will be ignored. If I find an example of spaces being necessary, I may improve the program, but for this use case, it works.

#### Alphabet

It supports multiple alphabets and will iterate through them. It must be formatted as one alphabet per line, ideally uppercase only. If no option is submitted, it will default to ABC...XYZ.

#### Dictionary

A dictionary is required. The default is words_alpha.txt, which needs to be in the same directory as the program. The default can be changed with input options.

#### Partially Solved

Custom Formatting and examples have been included in folders. Custom formatting algorithm below.

| to signify the start, then a number of unknown chars.
| to signify the following number, which will be no. of known chars
| to signify the start of known chars
Write out known chars
Repeat until end
It ends after the last known char

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

Partially solved text with an optional offset in case the partial solved unconfirmed where it lies. The order is essential. Only one of the two below is required; if two are included, only one will be processed, which may cause issues.

```terminal
-s solved.txt
-s solved.txt 1
```

Alphabet File (required for only cyphers that have been encrypted with vigenere and custom alphabet)
If unsure if required, this supports multiple alphabets and will iterate through them when needed.
Formatting is just one alphabet per line, with no duplicate letters per line.

```terminal
-a alphabet.txt
```

Dictionary File will default to 'words_alpha.txt' if nothing is supplied.

```terminal
-d dictionary.txt
```

If nothing is supplied, the Output File will default to output.csv. The file cannot exist before starting the program.

```terminal
-o output.csv
```

#### Running Options

Layers and optional 'smart' option.
The layers will state how my cyphers have been encrypted. So, K1, K2, and K3 Kryptos are all one layer, as each only uses one cypher to encrypt the plaintext into the cypher text. Layers must be a number, but they can be any number. I suggest only 1 or 2.
The 'Smart' option is for tests that reduce the number of iterations needed. It is off by default and is not optimised, so I suggest keeping it off. If turned on, it will only be operational on the last layer.
Only one of the two commands below should be run.
For reference, if everything is enabled with no smart option left disabled, the attempt made will be estimated below:
Layers = 1, Attempts ~ 2.2 Million
Layers = 2, Attempts ~ 5,000,000 Million
Anything above two layers would require multithreading, a high-performance CPU, and a lot of time.

```terminal
-l 2
-l 2 smart
```

Specify the Cyphers Used. I recommend running './breaker —help c' first to see what all the cyphers are called. This will turn off cyphers not stated. Below are examples, but it can be any combination of any or all. This is case-sensitive, so run the help command if you know the spelling.
By default, all cyphers will be enabled; this is just if you want to reduce iterations.

```terminal
-c vigenere
-c vigenere skip
-c skip reverse
```

Specify Tests Used. I recommend running './breaker -help t' first to see what all the tests are called. This won't affect the if-solved test, which is separate. Tests also require smart layers to be active, which is off by default.

```terminal
-t ioc
-t frequency
```

### Current Limitations

1. Case-sensitive input/output. For the current cyphers, this should be fine, but it would be a minor problem if that functionality were needed.

2. The option key is more than one character. This can be fixed, but it is not needed for the program’s current size.

3. Brute Force alphabet. It is not implemented as it would take too long to iterate through.

4. It needs a partial solution. Expanding and adding more tests to remove this limitation is easy, and an English test may be added in the future.

5. Multithreading is not available at the moment; it may be implemented shortly, as it should significantly improve performance.

6. Limited options for test pass marks; currently, you need to edit the testers.c file with the changes you want to make. We may also change this to add more options in CLI when running the command.

7. There is only one output file, and it may break if the user edits it before the program is finished. To be safer, it may make CSV files dynamic and merge them at the end, but this still needs to be implemented. The input text/file cannot contain symbols; this was made this way as more of a precaution. It could be changed to add a random letter where there was a symbol, but I don't require this functionality. I have included K2 & K3 folders with all the formatted files needed if you would like to improve/test your implementation.

9. This is not a limitation; variable and function names are messy. I didn't have a naming convention planned out when starting. I retroactively fixed some, but it's still a bit of a mess.

### Exit Status Codes

If the program fails to finish, it should print an error message if it doesn't, use the command below to find the exit code.

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

This only supports two main cyphers, as they have been used for the other parts of Kryptos.

#### Vigenere

Used for Kryptos K1 & K2.
Requires alphabet and keyword.

#### Rotation Transformation

Used for Kryptos K3.
It requires rotation data and the size of the block to rotate around. This can be calculated by multiplying the row dimensions against each other for every rotation to get a 'skips' value. This is far simpler and means less iteration. You need to iterate through the skip value, which would be, at maximum, the length of the input text.

#### Reversed

Iterates between reversed and not in between cypher attempts

### Tests Included

#### Index of Coincidence

Split up into many parts; this can be used to guess if the text is in a specific language or to check key lengths for cyphers such as vignettes. It is set up to check key lengths but is not used, as it would need at least another algorithm to make it worthwhile. This will be useful in making the dictionary unnecessary.

#### Frequency Analysis

It provides the same purpose as the Index of Coincidence, except it is currently only set up to check for the English alphabet, not crucial lengths.

#### Solved function

They are used to check if the output of a decypher attempt matches the partially solved text.

## Full Examples

### K1 Kryptos

For this example, we will pretend we know part of the plain text. A folder called K1 is included; it has all the files needed to test this. To make it more realistic, we will run it with as many default settings as possible.

We would have had to work out that the alphabet would not be in the same order and that vigenere was to be used. Also, we would need to know part of the plain text. These are just the program’s limitations at the moment. Brute forcing an alphabet wouldn't be too difficult but would take far too long to run, so it was not implemented.

So, the commands below first compile the application (if you are in the correct directory) and then run it.

```terminal
make -B breaker

./breaker -i K1/K1_input.txt -a K1/K_alphabets.txt -s K1/K1_part_solved.txt
```

You should get the default output file "output.csv" in your directory.

If you have duplicate solutions, this is because it will have tested for not reversing the text, which is used when there is more than one layer.

### K2 & K3 Kryptos

These do not currently work due to limitation no. 8. I have still included the folder if you want to test it. To force it to work anyway, you would need to change any symbols in the input file to random chars and then change them back after finishing. 

### K4 Kryptos

I have included K4 Kryptos, which inspired this project. Feel free to mess with options, add cyphers, etc., to see if you can crack it.
