#ifndef STACK_H
#define STACK_H

#define MAX_SIZE 150
#define MAXELE 20
#define LINESIZE 1000
#define MAXTERM 50
#define MAXNONTERM 150
#define MAXRULES 120
#define TOTALRULES 102
#define PARSECOL 57

typedef struct {
    char lexeme[MAXTERM];
    ParseTreeNode *children[MAXELE];
    int numChildren;
} ParseTreeNode;

typedef struct {
    char lexeme[MAXTERM];
    ParseTreeNode *nodePointer;
} StackElement;

typedef struct {
    StackElement *items[MAX_SIZE];
    int top;
} Stack;

void initializeStack(Stack *stack);
int isEmpty(Stack *stack);
int isFull(Stack *stack);
void push(Stack *stack, StackElement *str);
StackElement* pop(Stack *stack);
StackElement* peek(Stack *stack);
void freeStack(Stack *stack);

#endif