#include "parseTreeStack.h"

typedef struct {
    char lexeme[MAXTERM];
    ParseTreeNode *nodePointer;
} StackElement;

typedef struct {
    StackElement *items[MAX_SIZE];
    int top;
} Stack;

Stack* initializeStack(Stack *stack);
int isEmpty(Stack *stack);
int isFull(Stack *stack);
void push(Stack *stack, StackElement *str);
StackElement* pop(Stack *stack);
StackElement* peek(Stack *stack);
void freeStack(Stack *stack);
StackElement *createNewStackElement (char lexeme[MAXTERM]);