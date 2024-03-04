#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "parserDef.h"

Stack* initializeStack(Stack *stack);
int isEmpty(Stack *stack);
int isFull(Stack *stack);
void push(Stack *stack, StackElement *str);
StackElement* pop(Stack *stack);
StackElement* peek(Stack *stack);
void freeStack(Stack *stack);

StackElement *createNewStackElement (char lexeme[MAXTERM]);
ParseTreeNode* createNewParseTreeNode(char *lex);

void freeStack(Stack *stack);
void printInOrder(ParseTreeNode *root);
bool present(char element[MAXTERM], char array[MAXELE][MAXTERM], int noOfEleInArray);
void populateTerminals();
void populateFirstFromAnother(int ffIndex1, int ffIndex2);
void populateFirstFromSingleElement(int ffIndex, char _element[MAXTERM]);
void populateFollowFromFirst(int ffIndex1, int ffIndex2);
void populateFollowFromFollow(int ffIndex1, int ffIndex2);
void populateFollowFromElement(int ffIndex, char _element[MAXTERM]);
void intialiseFFandLookup();

NTLookupEntry getNTLookup(char *nonTerminal);
void findFirst(int ffind, int grammarInd);
void populateFirst();
void findFollow(int ffIndex);
void populateFollow();
int getTerminalIndex(char *terminal);
bool isTerminal(char *element);
void createParseTable();
void fillSyncInParseTable();
void createParseTree(char **input) ;
void printTerminals();
void printFirstFollow();
void printGrammarRules();
void printParseTable();
char **populateInputStream();
