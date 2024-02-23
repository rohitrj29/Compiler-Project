#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAXELE 20
#define LINESIZE 1000
#define MAXTERM 50
#define MAXNONTERM 150
#define MAXRULES 120

int lineNumber = 0;

// Struct to store grammar rules
typedef struct
{
    int noOfElements;
    char leftElement[MAXTERM];
    char rightElements[MAXELE][MAXTERM];
    int ruleNo;
} GrammarRule;

// Struct to store NT lookup entry
typedef struct
{
    char nonTerminal[MAXTERM];
    int ffIndex;
    int grammarIndex;
} NTLookupEntry;

// Struct to store first and follow sets
typedef struct
{
    char nonTerminal[MAXTERM];
    char firstSet[MAXELE][MAXTERM];
    char followSet[MAXELE][MAXTERM];
    int noOfFirst;
    int noOfFollow;
    bool isEpsilon;
    bool visited;
} FirstFollow;

// Arrays to store data
GrammarRule grammarRule[MAXRULES];
NTLookupEntry NTLookup[MAXNONTERM];
FirstFollow firstFollow[MAXNONTERM];

int ffsize = 0;
int noOfNonTerminals = 0; // Added for storing the total number of non-terminals

// Function to initialize first and follow sets and NT lookup
void intialiseFFandLookup()
{
    int entry = 0;
    char prev[MAXTERM] = "";

    for (int i = 0; i < lineNumber; i++)
    {
        char nonterm[MAXTERM];
        strcpy(nonterm, grammarRule[i].leftElement);

        // Check if the current non-terminal is different from the previous one
        if (strcmp(prev, nonterm) != 0)
        {
            // Copy non-terminal to NTLookup entry
            strcpy(NTLookup[entry].nonTerminal, nonterm);
            NTLookup[entry].grammarIndex = i;
            NTLookup[entry].ffIndex = entry;
            strcpy(firstFollow[entry].nonTerminal, nonterm);

            // Update previous non-terminal
            strcpy(prev, nonterm);

            // Increment entry count
            entry++;
        }

        firstFollow[entry - 1].visited = false;
        // Check if the first element of the production rule is epsilon
        if (strcmp(grammarRule[i].rightElements[0], "eps") == 0)
            firstFollow[entry - 1].isEpsilon = true;
        else
            firstFollow[entry - 1].isEpsilon = false;
    }

    // Set the total number of non-terminals
    noOfNonTerminals = entry;
}


NTLookupEntry getNTLookup(char *nonTerminal)
{
    for (int i = 0; i < noOfNonTerminals; i++)
    {
        if (strcmp(NTLookup[i].nonTerminal, nonTerminal) == 0)
        {
            return NTLookup[i]; // Return the specific entry found
        }
    }

    // Handle the case where no entry is found
    // This could be an error condition, so you might want to handle it accordingly
    // For now, let's return a default initialized entry
    NTLookupEntry defaultEntry = {"", 0, 0}; // Or whatever default values you want to set
    return defaultEntry;
}


// find if element's isepsilon is true in NTLookup
// NTLookupEntry* searchInLookup(char element[]){
//     int i=0;
//     while(NTLookup[i].nonTerminal!=NULL){
//         if(strcmp(NTLookup[i].nonTerminal,element)==0){
//             return NTLookup[i];
//         }
//         i++;
//     }
//     return NULL;
// }


void findFirst(int ffind, int grammarInd){

for(int i=grammarInd; i<102;i++){
    if(strcmp(grammarRule[i].leftElement,NTLookup[ffind].nonTerminal)!=0){
        break;
    }
    int j=grammarRule[i].noOfElements;
     for(int k=0;k<j;k++)
     {

        //TODO: if the first is already calculated
        //copy all first elements to the current non terminal

        //if the element is a terminal
        if(grammarRule[i].rightElements[k][0]>='A' && grammarRule[i].rightElements[k][0]<='Z'  || strcmp(grammarRule[i].rightElements[k], "eps") == 0){
            strcpy(firstFollow[ffind].firstSet[firstFollow[ffind].noOfFirst++],grammarRule[i].rightElements[k]);
            break;
        }
        else{
                NTLookupEntry check= getNTLookup(grammarRule[i].rightElements[k]);
                if(firstFollow[check.ffIndex].isEpsilon) {
                    
                    if(firstFollow[check.ffIndex].visited==true){
                        //populate
                    }
                    else{
                    findFirst(check.ffIndex,check.grammarIndex);
                    //populate first set
                    }
                    if(k==j-1){
                        //populate epsilon in first set
                        break;
                    }
                    
                    
                }
                else{
                    if(firstFollow[check.ffIndex].visited==true){
                        //populate
                    }
                    else{
                    findFirst(check.ffIndex,check.grammarIndex);
                    //populate first set
                    }
                    break;
                }
                
            }
            
        }
    
    }

    firstFollow[ffind].visited=true;
    return;

}

void populateFirst(){
    for(int i=0;i<ffsize;i++){
        firstFollow[i].noOfFirst=0;
        findFirst(i,NTLookup[i].grammarIndex);
    }
}


int main()
{
    FILE *fp;
    fp = fopen("./FinalGrammar.txt", "r");

    if (fp == NULL)
    {
        printf("File not found\n");
        return 1; // Return with error status
    }

    char line[LINESIZE];

    while (fgets(line, LINESIZE, fp) != NULL)
    {
        char *token = strtok(line, " ");
        int i = 0;

        while (token != NULL)
        {
            if (i == 0)
            {
                strcpy(grammarRule[lineNumber].leftElement, token);
            }
            else
            {
                strcpy(grammarRule[lineNumber].rightElements[i - 1], token);
            }
            i++;
            token = strtok(NULL, " ");
        }

        grammarRule[lineNumber].noOfElements = i - 1;
        grammarRule[lineNumber].ruleNo = lineNumber;
        lineNumber++;
    }

    fclose(fp);

    // print the grammar rules
    for (int i = 0; i < lineNumber; i++)
    {
        printf("%s ->   ", grammarRule[i].leftElement);
        for (int j = 0; j < grammarRule[i].noOfElements; j++)
        {
            printf("%s ", grammarRule[i].rightElements[j]);
        }
        printf("\n");
    }

    intialiseFFandLookup();

    for (int i = 0; i < noOfNonTerminals; i++)
    {
        printf("%s %d %d\n", NTLookup[i].nonTerminal, NTLookup[i].ffIndex, NTLookup[i].grammarIndex);
    }

    return 0;
}
