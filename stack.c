#include "stack.h"

void initializeStack(Stack *stack) {
    stack->top = -1;
}

int isEmpty(Stack *stack) {
    return (stack->top == -1);
}

int isFull(Stack *stack) {
    return (stack->top == MAX_SIZE - 1);
}

void push(Stack *stack, char* str) {
    if (isFull(stack)) {
        printf("Stack overflow!\n");
        return;
    }
    stack->top++;
    stack->items[stack->top] = strdup(str);
}


char* pop(Stack *stack) {
    if (isEmpty(stack)) {
        printf("Stack underflow!\n");
        return NULL;
    }
    return stack->items[stack->top--];
}


char* peek(Stack *stack) {
    if (isEmpty(stack)) {
        printf("Stack is empty!\n");
        return NULL;
    }
    return stack->items[stack->top];
}


void freeStack(Stack *stack) {
    while (!isEmpty(stack)) {
        free(pop(stack));
    }
}