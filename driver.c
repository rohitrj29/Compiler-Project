// TODO: include other headers
// TODO: change makefile

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "parseTreeStack.h"
#include <time.h>

void initialPrint();

int main(int argc, char *argv[])
{
    initialPrint();

    if (argc != 3)
    {
        printf("You have not entered the test case file name or the parseTreeOutput file name, please retry again!");
        exit(1);
    }

    char *testcaseFileName, parseTreeFileName;

    strcpy(testcaseFileName, argv[1]);
    strcpy(parseTreeFileName, argv[2]);

    // TODO: All printing to be double checked as per mail

    while (1)
    {
        int optionChosenByUser;
        scanf("%d", optionChosenByUser);

        switch (optionChosenByUser)
        {
        case 0:
            printf("Thanks for using our Compiler, have a nice day! \nExiting!");
            return 0;

        case 1:
            // Remove comments from code and print comment free code
            printf("Operation Compelete!");
            // TODO: Add these after every case
            break;

        case 2:
            // Prints all tokens and lexemes line number wise, dont pass it to parser and with pretty printing
            // Each token appears in a new line along with the corresponding lexeme and line number
            break;

        case 3:
            // Call lexical analyzer and pass it to parser and transfer the parse tree to specified file
            // Leftmost child ‐‐> parent node ‐‐> remaining siblings (excluding the leftmost)
            break;

        case 4:
            // For printing (on the console) the total time taken by project code
            printf("Starting Execution....\n");
            clock_t startTime = clock();

            // TODO: invoke your lexer and parser here, also see if idhar hi call krna h ya 3 press krne ke baad save krlena h

            clock_t endTime = clock();
            double totalCPUTime = (double)(endTime - startTime);
            double totalCPUTimeInSeconds = totalCPUTime / CLOCKS_PER_SEC;

            printf("Total CPU Time: %f , Total CPU Time in seconds: %f\n", totalCPUTime, totalCPUTimeInSeconds);
            break;

        default:
            printf("Invalid option chosen, please try again!");
            break;
        }
    }

    return 0;
}

void initialPrint()
{
    printf("Welcome to our compiler!");
    printf("Press 0 to exit");
    pritnf("Press 1 to print comment-free code");
    printf("Press 2 to print all tokens and lexemes");
    printf("Press 3 to compile the code and generate the Parse Tree");
    printf("Press 4 to compile code and print total time taken");
    printf("Please choose options 0-4 to start: ");
}