#include "stack.h"
#include "parseTreeStack.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

Stack *initializeStack(Stack *stack)
{
    stack = (Stack *)malloc(sizeof(Stack));
    stack->top = -1;
    return stack;
}

int isEmpty(Stack *stack)
{
    return (stack->top == -1);
}

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

void freeStack(Stack *stack)
{
    while (!isEmpty(stack))
    {
        free(pop(stack));
    }
}