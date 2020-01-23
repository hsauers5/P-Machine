#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DATA-STACK_HEIGHT 23
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVEL 3

int stack[MAX_DATA-STACK_HEIGHT];
int lex[MAX_LEXI_LEVEL];
int code[MAX_CODE_LENGTH];

/* Begin Stack */
/* ===========================================*/
int BP = 0; // base pointer
int SP = MAX_DATA-STACK_HEIGHT; // stack pointer

int stack_is_empty(void) {
    if (SP == MAX_DATA-STACK_HEIGHT) {
        return 1;
    } else {
        return 0;
    }
}

int stack_is_full(void) {
    if (SP == BP) {
        return 1;
    } else {
        return 0;
    }
}

int stack_peek(void) {
    return stack[SP];
}

int stack_pop(void) {
    if (!stack_is_empty()) {
        int tmp = stack[SP];
        SP += 1;
        return tmp;
    } else {
        printf("Stack is empty!");
    }
}

int stack_push(int data) {
    if (!stack_is_full()) {
        SP -= 1;
        stack[SP] = data;
        return 1;
    } else {
        printf("Stack is full!");
        return 0;
    }
}

/* End Stack */
/* ===========================================*/


/* Begin P-Machine */

int PC = 0;
int IR = 0;
int GP = -1;

// instruction format from file is {OP LEVEL M} space-separated.
typedef struct {
   int op;
   int l;
   int m;
}instruction;

/* find base L levels down */
// l stand for L in the instruction format
int base(int l, int base) {  
  int b1; 
  b1 = base; 
  while (l > 0) {
    b1 = stack[b1 - 1];
    l--;
  }
  return b1;
}


int do_operation(instruction * inst) {
    switch(inst->op) {
        
    }
}

int main(void) {
    printf("AAA");
}
