breaker:
	clang -ggdb3 -gdwarf-4 -O3 -std=c11 -Wall -Wextra -Werror -Wshadow -Wno-gnu-folding-constant -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -c -o breaker.o breaker.c
	clang -ggdb3 -gdwarf-4 -O3 -std=c11 -Wall -Wextra -Werror -Wshadow -Wno-gnu-folding-constant -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -c -o cyphers.o cyphers.c
	clang -ggdb3 -gdwarf-4 -O3 -std=c11 -Wall -Wextra -Werror -Wshadow -Wno-gnu-folding-constant -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -c -o testers.o testers.c
	clang -ggdb3 -gdwarf-4 -O3 -std=c11 -Wall -Wextra -Werror -Wshadow -Wno-gnu-folding-constant -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -c -o helpers.o helpers.c
	clang -ggdb3 -gdwarf-4 -O3 -std=c11 -Wall -Wextra -Werror -Wshadow -Wno-gnu-folding-constant -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -o breaker breaker.o cyphers.o testers.o helpers.o

# Personal Notes on Makefile/Clang config, keeping in file as all commented out and useful if I need to come back in a while.

# make -B breaker

# Clang Info/Notes:
# First three clang commands are prepping files.
# Final clang command is creating the file.
# Documentation at https://clang.llvm.org/get_started.html
# Debug Docs at https://gcc.gnu.org/onlinedocs/gcc/Debugging-Options.html
# ggdb3 is debugger with '3' adding more info
# gdwarf-4 provides debug info in DWARF format version 4
# O0 is no optimisation for debugging, final version I will change to -O3 for potential performance upgrade once confirmed working.
# std is what version of C I am coding in.
# Wall enables most warning messages to find errors that would still compile.
# Wextra more warning messages
# Werror changes all warnings into errors.
# Wshadow enables more warnings about variables.
# -o output file name. if not the output will be called a.out by default.

# -c option is to compile into object file (with a .o extension) for each source file. Used to compile multiple files before linking them together.


# May remove for the final version:
# Wno-gnu-folding-constant disables warning for non constant expressions. Useful while still coding
# Wno-sign-compare disables warnings when changing values between signed and unsigned. Potentially really bad if not accounted for in code.
# Wno-unused-parameter disables warnings for unused parameters. Useful while still coding.
# Wno-unused-variable disables warnings for for unused variables. Useful while still coding.
# Wno-unused-but-set-variable disables warnings for for unused variables. Useful while still coding.

# things to change in final version:
# change -O0 to -O3 for optimisation
# potentially add -Qunused-arguments to remove warnings if there are any about arguments
