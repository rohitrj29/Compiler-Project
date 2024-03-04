#ifndef PARSERDEF_H
#define PARSERDEF_H

#include <stdbool.h>

#define MAX_SIZE 250
#define MAXELE 20
#define LINESIZE 100000
#define MAXTERM 50
#define MAXNONTERM 150
#define MAXRULES 120
#define TOTALRULES 95
#define PARSECOL 57

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
    bool followCalc;
    bool lock;
} FirstFollow;

typedef struct ParseTreeNode
{
    char lexeme[MAXTERM];
    struct ParseTreeNode *children[MAX_SIZE];
    int numChildren;
    int outIndex;
    int nodeIndex;
    int parentIndex;
} ParseTreeNode;

#endif