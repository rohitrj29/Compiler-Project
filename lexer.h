#include "lexerDef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXSIZE 100
#define maxVarSize 100

extern int lexerLineNumber;

FILE *getStream(FILE *filePointer, TwinBuffer *twinBuffer);
int fromZeroToWhere(char currentCharacter);
TokenInfo *returnToken(TokenInfo *tokenInfo, char *value, char *tokenID, int lineNumber);
TokenInfo *getNextToken(TwinBuffer *twinBuffer, FILE *filePointer);
void runLexerOnly();