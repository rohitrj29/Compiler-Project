#ifndef LEXERDEF_H
#define LEXERDEF_H

#define bufferSize 4
#define SIZE 10000

typedef struct {
    char *value;
    char *tkId;
    int linenumber;
} TokenInfo;

//structure for the symbol table
typedef struct {
    char *tokId;
    char *entry;
} HashEntry;

//structure for twin buffer 
typedef struct {
    char buffer1[bufferSize];
    char buffer2[bufferSize];
    int forward;
    int lexBegin;
    int inUseBuffer; // 1 for First and 2 for Second
} TwinBuffer;

#endif
