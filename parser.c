#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "stack.h"
#include "parserDef.h"
#include "hashtable.h"
#include "lexerDef.h"
#include "lexer.h"

int parserLineNumber = 0;
int nodeIndexTracker = 1;

// Arrays to store data
GrammarRule grammarRule[MAXRULES];
//maps non terminal to its index in grammarRule and firstFollow
NTLookupEntry NTLookup[MAXNONTERM];
//stores first and follow sets of non terminals
FirstFollow firstFollow[MAXNONTERM];
//stores terminals
char terminals[PARSECOL][MAXTERM];
//stores the parse table
int parseTable[55][PARSECOL];

char **token;
char **value;
int *lineNo;

int noOfNonTerminals = 0; // Added for storing the total number of non-terminals

// Function to create a new parse tree node
ParseTreeNode *createNewParseTreeNode(char *lex)
{
    ParseTreeNode *treeElement = (ParseTreeNode *) malloc(sizeof(ParseTreeNode));
    treeElement->children[0] = NULL;
    strcpy(treeElement -> parent,"-----");
    treeElement -> nodeIndex = nodeIndexTracker ++;
    strcpy(treeElement->lexeme, lex);
    treeElement->numChildren = 0;
    treeElement->outIndex = -1;
    return treeElement;
}


// Function to print the parse tree in inorder traversal
void printInorder(FILE *fp, ParseTreeNode *root)
{
    if (root->lexeme == NULL)
    {
        return;
    }

    // Traverse each child node
    for (int i = root->numChildren - 1; i > 0; i--)
    {
        printInorder(fp, root->children[i]);
    }
    if (root->outIndex >= 0)
    {
        // Print information for the current node
        // "Lexeme", "LINE NO", "TOKEN NAME", "VALUE IF NUM", "PARENT SYMBOL", "IS LEAF NODE","NODE SYMBOL"
        fprintf(fp,"%-30s %-15d %-22s %-22s %-22s %-15s %-22s\n",

        value[root->outIndex],
        lineNo[root->outIndex],
        root->lexeme, // Replace with the actual value if number logic
        (strcmp(root->lexeme,"TK_NUM")==0||strcmp(root->lexeme,"TK_RNUM")==0)?value[root->outIndex]:"-----",
        root -> parent,
        (root->numChildren == 0) ? "Yes" : "No",
        "-----"); 
    }
    else
    {
        // Print information for the current node
        fprintf(fp, "%-30s %-15s %-22s %-22s %-22s %-14s %-22s\n",
        "-----",
        "-----",
        "-----",
        "-----", // Replace with the actual value if number logic
        root -> parent,
        (root->numChildren == 0) ? "Yes" : "No",
        root->lexeme); 
    }

    // value , line no, token
    printInorder(fp, root->children[0]);
}

// Function to print the parse tree in inorder traversal
void printParseTree(ParseTreeNode *root, char *fileName) {
    FILE *fp;
    fp = fopen(fileName, "w");
    fprintf(fp,"%-30s %-15s %-22s %-22s %-22s %-15s %-22s\n", "Lexeme", "LINE NO", "TOKEN NAME", "VALUE IF NUM", "PARENT SYMBOL", "IS LEAF NODE","NODE SYMBOL");
    fprintf(fp,"%-30s %-15s %-22s %-22s %-22s %-15s %-22s\n", "------", "-------", "-----------", "------------", "-------------", "-------------","------------");
    printInorder(fp, root);

    fclose(fp);
}

// Function to check if an element is present in an array
bool present(char element[MAXTERM], char array[MAXELE][MAXTERM], int noOfEleInArray)
{
    for (int i = 0; i < noOfEleInArray; i++)
    {
        if (strcmp(element, array[i]) == 0)
            return true;
    }

    return false;
}

// Function to populate the terminals array
void populateTerminals()
{
    int index = 1;
    strcpy(terminals[0], "$");
    for (int i = 0; i < TOTALRULES; i++)
    {
        for (int j = 0; j < grammarRule[i].noOfElements; j++)
        {
            if ((grammarRule[i].rightElements[j][0] == 'T') && present(grammarRule[i].rightElements[j], terminals, 58) == false)
            {
                strcpy(terminals[index], grammarRule[i].rightElements[j]);
                index++;
            }
        }
    }
}

// Function to populate the first set of a non-terminal from another non-terminal
void populateFirstFromAnother(int ffIndex1, int ffIndex2)
{
    int initialElementsInFirst = firstFollow[ffIndex1].noOfFirst, intialElementsInFF2 = firstFollow[ffIndex2].noOfFirst;

    for (int i = 0; i < intialElementsInFF2; i++)
    {
        char element[MAXTERM];
        strcpy(element, firstFollow[ffIndex2].firstSet[i]);

        if (present(element, firstFollow[ffIndex1].firstSet, initialElementsInFirst) || strcmp(element, "eps") == 0)
            continue;
        else
        {
            strcpy(firstFollow[ffIndex1].firstSet[initialElementsInFirst++], element);
            firstFollow[ffIndex1].noOfFirst++;
        }
    }
}

// Function to populate the first set of a non-terminal from a single element say epsilon or terminal
void populateFirstFromSingleElement(int ffIndex, char _element[MAXTERM])
{
    int initialElementsInFirst = firstFollow[ffIndex].noOfFirst;
    char element[MAXTERM];
    strcpy(element, _element);

    if (present(element, firstFollow[ffIndex].firstSet, initialElementsInFirst))
        return;
    else
    {
        strcpy(firstFollow[ffIndex].firstSet[initialElementsInFirst], element);
        firstFollow[ffIndex].noOfFirst++;
    }
}

// Function to populate the follow set of a non-terminal from first set of another non-terminal
void populateFollowFromFirst(int ffIndex1, int ffIndex2)
{
    int initialElementsInFollow = firstFollow[ffIndex1].noOfFollow, intialElementsInFirst = firstFollow[ffIndex2].noOfFirst;

    for (int i = 0; i < intialElementsInFirst; i++)
    {
        char element[MAXTERM];
        strcpy(element, firstFollow[ffIndex2].firstSet[i]);

        if (present(element, firstFollow[ffIndex1].followSet, initialElementsInFollow) || strcmp(element, "eps") == 0)
            continue;
        else
        {
            strcpy(firstFollow[ffIndex1].followSet[initialElementsInFollow++], element);
            firstFollow[ffIndex1].noOfFollow++;
        }
    }
}

// Function to populate the follow set of a non-terminal from follow set of another non-terminal
void populateFollowFromFollow(int ffIndex1, int ffIndex2)
{
    int initialElementsInFollow1 = firstFollow[ffIndex1].noOfFollow, intialElementsInFollow2 = firstFollow[ffIndex2].noOfFollow;

    for (int i = 0; i < intialElementsInFollow2; i++)
    {
        char element[MAXTERM];
        strcpy(element, firstFollow[ffIndex2].followSet[i]);

        if (present(element, firstFollow[ffIndex1].followSet, initialElementsInFollow1) || strcmp(element, "eps") == 0)
            continue;
        else
        {
            strcpy(firstFollow[ffIndex1].followSet[initialElementsInFollow1++], element);
            firstFollow[ffIndex1].noOfFollow++;
        }
    }
}


// Function to populate the follow set of a non-terminal from a single element say epsilon or terminal
void populateFollowFromElement(int ffIndex, char _element[MAXTERM])
{
    int initialElementsInFollow = firstFollow[ffIndex].noOfFollow;
    char element[MAXTERM];
    strcpy(element, _element);

    if (present(element, firstFollow[ffIndex].followSet, initialElementsInFollow) || strcmp(element, "eps") == 0)
        return;
    else
    {
        strcpy(firstFollow[ffIndex].followSet[initialElementsInFollow], element);
        firstFollow[ffIndex].noOfFollow++;
    }
}

// Function to initialize first and follow sets and NT lookup
void intialiseFFandLookup()
{
    int entry = 0;
    char prev[MAXTERM] = "";
    // Loop through the grammar rules
    for (int i = 0; i < parserLineNumber; i++)
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
        firstFollow[entry - 1].followCalc = false;
        // Check if the first element of the production rule is epsilon
        if (strcmp(grammarRule[i].rightElements[0], "eps") == 0)
            firstFollow[entry - 1].isEpsilon = true;
        else
            firstFollow[entry - 1].isEpsilon = false;
    }

    // Set the total number of non-terminals
    noOfNonTerminals = entry;
}


// Function to get the NTLookup entry for a non-terminal
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

// Function to find the first set of a non-terminal
void findFirst(int ffind, int grammarInd)
{

    for (int i = grammarInd; i < TOTALRULES; i++)
    {
        if (strcmp(grammarRule[i].leftElement, NTLookup[ffind].nonTerminal) != 0)
        {
            break;
        }

        int j = grammarRule[i].noOfElements;
        for (int k = 0; k < j; k++)
        {
            
            // TODO: if the first is already calculated - Done
            // copy all first elements to the current non terminal- Done

            // if the element is a terminal
            if ((grammarRule[i].rightElements[k][0] == 'T') || strcmp(grammarRule[i].rightElements[k], "eps") == 0)
            {
                populateFirstFromSingleElement(ffind, grammarRule[i].rightElements[k]);
                break;
            }
            else
            {
                NTLookupEntry check = getNTLookup(grammarRule[i].rightElements[k]);
                if (firstFollow[check.ffIndex].isEpsilon)
                {

                    if (firstFollow[check.ffIndex].visited == true)
                    {
                        // populate
                        populateFirstFromAnother(ffind, check.ffIndex);
                    }
                    else
                    {
                        findFirst(check.ffIndex, check.grammarIndex);
                        // populate first set
                        populateFirstFromAnother(ffind, check.ffIndex);
                    }
                    if (k == j - 1)
                    {
                        // populate epsilon in first set
                        populateFirstFromSingleElement(ffind, "eps");
                        break;
                    }
                }
                else
                {
                    if (firstFollow[check.ffIndex].visited == true)
                    {
                        // populate
                        populateFirstFromAnother(ffind, check.ffIndex);
                    }
                    else
                    {
                        findFirst(check.ffIndex, check.grammarIndex);
                        // populate first set
                        populateFirstFromAnother(ffind, check.ffIndex);
                    }
                    break;
                }
            }
        }
    }

    firstFollow[ffind].visited = true;
    return;
}

// Function to find the follow set of a non-terminal
void findFollow(int ffIndex)
{
    for (int i = 0; i < parserLineNumber; i++)
    {
        int found = 0;

        for (int j = 0; j < grammarRule[i].noOfElements; j++)
        {
            if (strcmp(grammarRule[i].rightElements[j], firstFollow[ffIndex].nonTerminal) == 0)
            {
                found = 1;
            }
            if (found == 1)
            {

                // last element
                if (strcmp(grammarRule[i].rightElements[j], firstFollow[ffIndex].nonTerminal) == 0 && j == grammarRule[i].noOfElements - 1)
                {

                    NTLookupEntry check = getNTLookup(grammarRule[i].leftElement);
                    if (firstFollow[check.ffIndex].lock == true)
                    {
                        found = 0;
                        continue;
                    }
                    if (strcmp(grammarRule[i].leftElement, grammarRule[i].rightElements[grammarRule[i].noOfElements - 1]) == 0)
                    {
                        break;
                    }
                    firstFollow[ffIndex].lock = true;
                    findFollow(check.ffIndex);
                    firstFollow[ffIndex].lock = false;
                    // populate follow set with follow set of left element
                    populateFollowFromFollow(ffIndex, check.ffIndex);
                    found = 0;
                }
                // not the last element
                else
                {
                    if (grammarRule[i].rightElements[j + 1][0] == 'T')
                    {
                        // populate follow set with rightElement[j+1] as it is a terminal
                        populateFollowFromElement(ffIndex, grammarRule[i].rightElements[j + 1]);
                        found = 0;
                        break;
                    }
                    else
                    {
                        NTLookupEntry check = getNTLookup(grammarRule[i].rightElements[j + 1]);
                        if (firstFollow[check.ffIndex].lock == true)
                        {
                            found = 0;
                            continue;
                        }
                        if (firstFollow[check.ffIndex].isEpsilon)
                        {


                            if (j + 1 == grammarRule[i].noOfElements - 1)
                            {

                                // populate follow set with the first of last element i.e. j+1 current
                                populateFollowFromFirst(ffIndex, check.ffIndex);
                                NTLookupEntry check = getNTLookup(grammarRule[i].leftElement);
                                firstFollow[ffIndex].lock = true;
                                findFollow(check.ffIndex);
                                firstFollow[ffIndex].lock = false;
                                // populate follow set with follow set of left element
                                populateFollowFromFollow(ffIndex, check.ffIndex);
                                found = 0;
                            }
                            else
                            {

                                // populate the follow set with the first of rightElement[j+1] -eps we are currently at
                                populateFollowFromFirst(ffIndex, check.ffIndex);
                                firstFollow[ffIndex].lock = true;
                                findFollow(check.ffIndex);
                                firstFollow[ffIndex].lock = false;
                            }
                            // }
                        }
                        else
                        {
                            // populate follow set with first set of check.nonTerminal
                            populateFollowFromFirst(ffIndex, check.ffIndex);
                            found = 0;
                        }
                    }
                }
            }
        }
    }
    firstFollow[ffIndex].followCalc = true;
    return;
}

/*
parser table population
using first and follow set of non terminals
*/

// pass a terminal and get its index
int getTerminalIndex(char *terminal)
{
    for (int i = 0; i < PARSECOL; i++)
    {
        if (strcmp(terminals[i], terminal) == 0)
        {
            return i;
        }
    }
    return -1;
}

// check if the element is terminal
bool isTerminal(char *element)
{
    if ((element[0] == 'T') || element[0] == '$')
    {
        return true;
    }
    return false;
}

// fill sync in parse table
void fillSyncInParseTable()
{
    for (int i = 0; i < noOfNonTerminals; i++)
    {

        for (int k = 0; k < firstFollow[i].noOfFollow; k++)
        {
            int followIndex = getTerminalIndex(firstFollow[i].followSet[k]);
            if (parseTable[i][followIndex] == -1)
                parseTable[i][followIndex] = -2;
        }
        // for(int k = 0; k < firstFollow[i].noOfFirst; k++)
        // {
        //     int firstIndex = getTerminalIndex(firstFollow[i].firstSet[k]);
        //     if(strcmp(firstFollow[i].firstSet[k],"eps")==0)
        //     continue;
        //     if(parseTable[i][firstIndex] == -1)
        //         parseTable[i][firstIndex] = -2;
        // }
    }
}

// create parse table
void createParseTable()
{
    memset(parseTable, -1, sizeof(parseTable));

    for (int gindex = 0; gindex < TOTALRULES; gindex++)
    {   
        // get the index of the non terminal
        NTLookupEntry entry = getNTLookup(grammarRule[gindex].leftElement);
        int ntindex = entry.ffIndex;
        FirstFollow ff = firstFollow[ntindex];
        GrammarRule rule = grammarRule[gindex];
        // ntindex=10
        // only right[0] needs to be checked

        //flag is to check if we have to move to the next right element 
        int flag = 1;

        for (int rhsIndex = 0; flag == 1 && rhsIndex < rule.noOfElements; rhsIndex++)
        {
            flag = 0;
            char element[MAXTERM];
            strcpy(element, rule.rightElements[rhsIndex]);
            // if the element is epsilon
            if (strcmp(element, "eps") == 0)
            {
                for (int findex = 0; findex < ff.noOfFollow; findex++)
                {
                    char followElement[MAXTERM];
                    strcpy(followElement, ff.followSet[findex]);
                    int followIndex = getTerminalIndex(followElement);
                    if (followIndex == 56)
                    {
                        bool test = true;
                    }
                    parseTable[ntindex][followIndex] = gindex;
                }
            }
            else if (isTerminal(element))
            {
                // variable is terminal
                int terminalIndex = getTerminalIndex(element);
                if (terminalIndex == 56)
                {
                    bool test = true;
                }
                parseTable[ntindex][terminalIndex] = gindex;
            }
            else
            {
                // variable is non-terminal
                int nonTerminalIndex = getNTLookup(element).ffIndex;
                //loop the first set of the non terminal
                for (int findex = 0; findex < firstFollow[nonTerminalIndex].noOfFirst; findex++)
                {

                    char firstElement[MAXTERM];
                    strcpy(firstElement, firstFollow[nonTerminalIndex].firstSet[findex]);
                    //if the first contains epsilon
                    if (strcmp(firstElement, "eps") == 0)
                    {
                        // first of next element until not eps is matched
                        flag = 1;
                        if (rhsIndex == rule.noOfElements - 1)
                        {
                            for (int findex = 0; findex < ff.noOfFollow; findex++)
                            {
                                char followElement[MAXTERM];
                                strcpy(followElement, ff.followSet[findex]);
                                int followIndex = getTerminalIndex(followElement);
                                if (followIndex == 56)
                                {
                                    bool test = true;
                                }
                                parseTable[ntindex][followIndex] = gindex;
                            }
                            flag = 0;
                        }
                    }
                    int firstIndex = getTerminalIndex(firstElement);
                    if (firstIndex == 56)
                    {
                        bool test = true;
                    }
                    if (firstIndex != -1)
                        parseTable[ntindex][firstIndex] = gindex;
                }
            }
        }
    }
    //populate the sync set in the parse table
    fillSyncInParseTable();
}

// Function to parse the output returned by the lexer
void parseInputSourceCode(char *parseTreeFileName) {
    Stack *myStack;
    myStack = initializeStack(myStack);
    int ind = 0;
    char currtoken[MAXTERM];
    strcpy(currtoken, token[ind++]);

    ParseTreeNode *root = createNewParseTreeNode(grammarRule[0].leftElement);
    root->outIndex = -1;
    StackElement *dollar = createNewStackElement("$");
    // push the dollar sign to the stack to indicate the end
    push(myStack, dollar);

    StackElement *startSymbolEle = createNewStackElement(grammarRule[0].leftElement);
    startSymbolEle->nodePointer = root;
    //push the start symbol to the stack
    push(myStack, startSymbolEle);

    int success = 1;
    while (strcmp(currtoken, "EOF") != 0)
    {

        // printing lexical error

        if (strcmp(currtoken, "ERROR") == 0)
        {
            success = 0;
            printf("%s  \n", value[ind - 1]);
            strcpy(currtoken, token[ind++]);
            continue;
        }

        StackElement *topElement = peek(myStack);
        // index of token in the parse table and index of topElement in the parse table
        int tokenIndex = getTerminalIndex(currtoken);
        int topElementIndex = -1;
        if (!isTerminal(topElement->lexeme))
        {
            topElementIndex = getNTLookup(topElement->lexeme).ffIndex;
        }

        if (strcmp(currtoken, "TK_READ") == 0)
        {
            bool test = true;
        }

        ParseTreeNode *currTreePointer = topElement->nodePointer;

        int tableValue = -3;
        //if the entry is valid get the value from the parse table
        if (topElementIndex >= 0)
            tableValue = parseTable[topElementIndex][tokenIndex];

        //if the top of the stack is a terminal and the token matches the top of the stack
        if (isTerminal(currtoken) && isTerminal(topElement->lexeme) && strcmp(currtoken, topElement->lexeme) == 0)
        {
            topElement->nodePointer->outIndex = ind - 1;
            strcpy(currtoken, token[ind++]);
            pop(myStack);
            free(topElement);
        }
        else if (topElement->lexeme[0] == '$')
        { 
            //if the top of the stack is a dollar sign 
            break;
        }
        else if (topElementIndex == -1)
        {   
            //if the top of the stack is a terminal and the token doesn't match the top of the stack
            success = 0;
            printf("Line %d Error: The token is %s for lexeme %s doesn't match the expected token %s \n", lineNo[ind - 1], currtoken, value[ind - 1], topElement->lexeme);
            // strcpy(currtoken, token[ind ++]);
            pop(myStack);
            free(topElement);
        }
        else if (tableValue == -2)
        {
            //sync encountered
            success = 0;
            if (strlen(topElement->lexeme) > 2 && topElement->lexeme[0] == 'T' && topElement->lexeme[1] == 'K')
                printf("Line  Invalid token %s encountered on the stack top %s \n", topElement->lexeme);
            pop(myStack);
            free(topElement);
        }
        else if (tableValue == -1)
        {
            //error encountered
            success = 0;
            printf("Line %d Error: token skipped %s with value %s  \n", lineNo[ind - 1], currtoken, value[ind - 1]);

            strcpy(currtoken, token[ind++]);
        }
        else if (tableValue >= 0)
        {
            //if the top of the stack is a non terminal 
            GrammarRule rule = grammarRule[tableValue];
            char par[MAXTERM];
            strcpy(par,topElement->nodePointer->lexeme);
            pop(myStack);
            free(topElement);
            
            //push the rules from left to right to the stack
            for (int i = rule.noOfElements - 1; i >= 0; i--) {
                if(strcmp(rule.rightElements[i], "eps") == 0) 
                {   
                    //printf("%s\n",myStack->items[myStack->top]->lexeme);
                    // pop(myStack);
                    // free(topElement);
                    continue;
                }

                StackElement *newElement = createNewStackElement(rule.rightElements[i]);
                ParseTreeNode *treeNode = createNewParseTreeNode(rule.rightElements[i]);

                newElement->nodePointer = treeNode;

                int originalChildren = currTreePointer -> numChildren;
                currTreePointer -> children[originalChildren] = treeNode;
                strcpy(treeNode -> parent ,par);
                currTreePointer -> numChildren ++;

                push(myStack, newElement);
            }
        }
        else
        {
            /*error cases
            1. if both the top of the stack and token is a terminal is not equal to it
            */
            success = 0;
            break;
        }
    }

    //if error dont print the parse tree
    if (!success)
    {
        return;
    } else {
        //if no error print the parse tree
        printf("Both lexical and syntax analysis modules implemented!\n"); 
        printf("Input Source Code is syntactically correct ........\n");

        printParseTree(root, parseTreeFileName);
        
        printf("Parse Tree was constructed and the inorder traversal of the parse tree is saved in the file name provided! \n\n");
    }

    // if stack is not empty pop it out
    free(myStack);
}

/*
end of parser table population
*/

void printTerminals()
{
    // print terminals
    for (int i = 0; i < PARSECOL; i++)
    {
        printf(" %d %s \n", i, terminals[i]);
    }
}

// Function to print the first and follow sets
void printFirstFollow()
{
    for (int i = 0; i < noOfNonTerminals; i++)
    {
        printf("First set of %s \n ", firstFollow[i].nonTerminal);
        for (int j = 0; j < firstFollow[i].noOfFirst; j++)
        {
            printf("%s ", firstFollow[i].firstSet[j]);
        }
        printf("\n");
        printf("Follow set of %s \n ", firstFollow[i].nonTerminal);
        for (int j = 0; j < firstFollow[i].noOfFollow; j++)
        {
            printf("%s ", firstFollow[i].followSet[j]);
        }
        printf("\n");
    }
}

void printGrammarRules()
{
    // print the grammar rules
    for (int i = 0; i < parserLineNumber; i++)
    {
        printf("%s ->   ", grammarRule[i].leftElement);
        for (int j = 0; j < grammarRule[i].noOfElements; j++)
        {
            printf("%s ", grammarRule[i].rightElements[j]);
        }
        printf("\n");
    }
}

// Function to print the parse table
void printParseTable()
{
    printf("Parse Table\n");
    printf("Non Terminals ");
    for (int i = 0; i < PARSECOL; i++)
    {
        printf("%s ", terminals[i]);
    }
    printf("\n");
    for (int i = 0; i < noOfNonTerminals; i++)
    {
        printf("%s ", firstFollow[i].nonTerminal);
        for (int j = 0; j < PARSECOL; j++)
        {
            if (parseTable[i][j] == -1)
            {
                printf("error ");
            }
            else if (parseTable[i][j] == -2)
            {
                printf("sync ");
            }
            else
            {
                printf("%d ", parseTable[i][j]);
            }
        }
        printf("\n");
    }
}


void computeFirstAndFollowSets()
{
    // populate first of all non terminals
    for (int i = 0; i < noOfNonTerminals; i++)
    {
        if (firstFollow[i].visited == true)
        {
            continue;
        }
        firstFollow[i].noOfFirst = 0;
        findFirst(i, NTLookup[i].grammarIndex);
    }
    // populate follow of all non terminals
    firstFollow[0].noOfFollow = 1;
    strcpy(firstFollow[0].followSet[0], "$");
    for (int i = 1; i < noOfNonTerminals; i++)
    {
        firstFollow[i].noOfFollow = 0;
        findFollow(i);
    }
}

// Function to start parsing
void startParsing()
{
    FILE *fp;
    fp = fopen("./grammar.txt", "r");

    if (fp == NULL)
    {
        printf("File not found\n");
        exit(1); // Return with error status
    }

    char line[LINESIZE];

    while (fgets(line, LINESIZE, fp) != NULL)
    {
        char *token = strtok(line, " ");

        int i = 0;

        while (token != NULL)
        {

            int len = strlen(token);
            if (token[len - 1] == '\n')
            {
                token[len - 1] = '\0';
            }

            if (i == 0)
            {
                strcpy(grammarRule[parserLineNumber].leftElement, token);
            }
            else
            {
                strcpy(grammarRule[parserLineNumber].rightElements[i - 1], token);
            }
            i++;
            token = strtok(NULL, " ");
        }

        grammarRule[parserLineNumber].noOfElements = i - 1;
        grammarRule[parserLineNumber].ruleNo = parserLineNumber;
        parserLineNumber++;
    }

    fclose(fp);

    intialiseFFandLookup();
    populateTerminals();

    computeFirstAndFollowSets();

    printf("FIRST and FOLLOW set automated\n");

    createParseTable();
    
    printf("Parse Table Created!\n");

    return;
}

// Function to run the lexer and parser
void runLexerAndParser(char *fileName, char *parseTreeFileName)
{
    // Initialize File Pointer
    FILE *filePointer;
    filePointer = fopen(fileName, "r");

    if (filePointer == NULL)
    {
        printf("Failed to open file!\n");
        exit(1);
    }

    startParsing();
    // Initialize Twin Buffer
    TwinBuffer *twinBuffer = (TwinBuffer *)malloc(sizeof(TwinBuffer));
    twinBuffer->forward = bufferSize;
    twinBuffer->lexBegin = 0;
    twinBuffer->inUseBuffer = 2;

    // create new hashtable and initialise with keywords
    HashMap *myMap = initializeHashMap();
    // Call function to get buffer
    filePointer = getStream(filePointer, twinBuffer);
    TokenInfo *tkinfo;

    int capacity = MAX_SIZE;

    token = (char **)malloc(sizeof(char *) * MAX_SIZE);
    value = (char **)malloc(sizeof(char *) * MAX_SIZE);
    lineNo = (int *)malloc(sizeof(int) * MAX_SIZE);

    int ind = 0;
    while (filePointer != NULL)
    {

        tkinfo = getNextToken(twinBuffer, filePointer);
        if (tkinfo->tkId == NULL)
        {
            // printf("%s\n",tkinfo->value);
            lineNo[ind] = lexerLineNumber + 1;
            token[ind] = (char *)malloc(sizeof(char) * MAXTERM);
            strcpy(token[ind], "ERROR");
            value[ind] = (char *)malloc(sizeof(char) * MAXTERM);
            strcpy(value[ind], tkinfo->value);
        }
        else if (strcmp(tkinfo->value, "$") == 0)
        {
            // printf("%s \n",tkinfo->value);
            lineNo[ind] = lexerLineNumber + 1;
            token[ind] = (char *)malloc(sizeof(char) * MAXTERM);
            strcpy(token[ind], tkinfo->tkId);
            value[ind] = (char *)malloc(sizeof(char) * MAXTERM);
            strcpy(value[ind], tkinfo->value);
            break;
        }
        else
        {
            if ((strlen(tkinfo->value) <= 20) | (strcmp(tkinfo->tkId, "TK_FUNID") == 0 & strlen(tkinfo->value) <= 30))
            {
                if (strcmp(getValue(myMap, tkinfo->value), "KEY NOT FOUND") == 0)
                {
                    insertIntoHash(myMap, tkinfo->value, tkinfo->tkId);
                }

                // printf("Line %d Lexeme %s  Token %s \n", lexerLineNumber+1,tkinfo->value, getValue(myMap,tkinfo->value));
                lineNo[ind] = lexerLineNumber + 1;
                token[ind] = (char *)malloc(sizeof(char) * MAXTERM);
                strcpy(token[ind], getValue(myMap, tkinfo->value));
                value[ind] = (char *)malloc(sizeof(char) * MAXTERM);
                strcpy(value[ind], tkinfo->value);
            }
            else
            {
                lineNo[ind] = lexerLineNumber + 1;
                token[ind] = (char *)malloc(sizeof(char) * MAXTERM);
                strcpy(token[ind], "ERROR");
                value[ind] = (char *)malloc(sizeof(char) * MAXTERM);

                char *errMessage = (char *)malloc(sizeof(char) * MAXSIZE);

                strcpy(errMessage, "Line ");
                char str[5];
                sprintf(str, "%d", lexerLineNumber + 1);
                strcat(errMessage, str);

                char str1[100] = " Error: Identifier is too long ";
                strcat(errMessage, str1);

                strcpy(value[ind], errMessage);
                // printf("Line %d Error:  Identifier is too long\n", lexerLineNumber+1);
            }
        }
        ind++;
        if (ind == capacity)
        {
            capacity = capacity * 2;
            token = (char **)realloc(token, sizeof(char *) * capacity);
            value = (char **)realloc(value, sizeof(char *) * capacity);
            lineNo = (int *)realloc(lineNo, sizeof(int) * capacity);
        }
    }


    parseInputSourceCode(parseTreeFileName);
    // Free the memory and set all global variables to 0
    lexerLineNumber=0;
    parserLineNumber=0;
    free(token);
    free(value);
    free(lineNo);
    destroyHashMap(myMap);
    fclose(filePointer);
    return;
}