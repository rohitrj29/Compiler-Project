#include "stack.h"
#include "parseTreeStack.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// Function to initialize Stack
Stack *initializeStack(Stack *stack)
{
    stack = (Stack *)malloc(sizeof(Stack));
    stack->top = -1;
    return stack;
}

// Function to check if stack is empty
int isEmpty(Stack *stack)
{
    return (stack->top == -1);
}

// Function to check if stack is full or not
int isFull(Stack *stack)
{
    return (stack->top == MAX_SIZE - 1);
}

void push(Stack *stack, StackElement *stackEle)
{
    if (isFull(stack))
    {
        printf("Stack overflow!\n");
        return;
    }

    stack->top++;
    stack->items[stack->top] = stackEle;
}

StackElement *pop(Stack *stack)
{
    if (isEmpty(stack))
    {
        printf("Stack underflow!\n");
        return NULL;
    }

    return stack->items[stack->top--];
}

// Function to see top element of stack
StackElement *peek(Stack *stack)
{
    if (isEmpty(stack))
    {
        printf("Stack is empty!\n");
        return NULL;
    }

    return stack->items[stack->top];
}

StackElement *createNewStackElement(char lexeme[MAXTERM])
{
    StackElement *stackElement = (StackElement *)malloc(sizeof(StackElement));
    stackElement->nodePointer = NULL;
    strcpy(stackElement->lexeme, lexeme);

    return stackElement;
}

// Functio to de-allocate memory
void freeStack(Stack *stack)
{
    while (!isEmpty(stack))
    {
        free(pop(stack));
    }
}