#!/bin/bash

# Compile symbolTable.c
gcc -o symbolTable symbolTable.c

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "symbolTable.c compiled successfully."
    
    # Run the compiled program
    ./symbolTable
else
    echo "Error: Compilation of symbolTable.c failed."
fi

# Compile lexer.c
gcc -o lexer lexer.c

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "lexer.c compiled successfully."
    
    # Run the compiled program
    ./lexer
else
    echo "Error: Compilation of lexer.c failed."
fi
