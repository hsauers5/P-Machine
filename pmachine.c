#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DATA-STACK_HEIGHT 23
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVEL 3

int stack[MAX_DATA-STACK_HEIGHT];
int lex[MAX_LEXI_LEVEL];
int code[MAX_CODE_LENGTH];

int stack_top = -1;

int stack_is_empty(void) {
    if (stack_top == -1) {
        return 1;
    } else {
        return 0;
    }
}

int stack_is_full(void) {
    if (stack_top == MAX_DATA-STACK_HEIGHT) {
        return 1;
    } else {
        return 0;
    }
}

int stack_peek(void) {
    return stack[stack_top];
}

int stack_pop(void) {
    if (!stack_is_empty()) {
        int tmp = stack[stack_top];
        stack_top -= 1;
        return tmp;
    } else {
        printf("Stack is empty!");
    }
}

int stack_push(int data) {
    if (!stack_is_full()) {
        top += 1;
        stack[stack_top] = data;
        return 1;
    } else {
        printf("Stack is full!");
        return 0;
    }
}


int main(void) {
    printf("AAA");
}
