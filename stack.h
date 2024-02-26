#ifndef STACK_H
#define STACK_H

#define MAX_SIZE 150

typedef struct {
    char* items[MAX_SIZE];
    int top;
} Stack;

void initializeStack(Stack *stack);
int isEmpty(Stack *stack);
int isFull(Stack *stack);
void push(Stack *stack, char* str);
char* pop(Stack *stack);
char* peek(Stack *stack);
void freeStack(Stack *stack);

#endif