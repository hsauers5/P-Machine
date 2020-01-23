#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DATA_STACK_HEIGHT 23
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVEL 3

int stack[MAX_DATA_STACK_HEIGHT];
int lex[MAX_LEXI_LEVEL];
int code[MAX_CODE_LENGTH];

// strangely: if halt is false then program should halt, else continue
int halt = -1;

// registers
int PC = 0;
int IR = 0;
int GP = -1;

/* Begin Stack */
/* ===========================================*/
// these are also registers
int BP = 0; // base pointer
int SP = MAX_DATA_STACK_HEIGHT; // stack pointer

int stack_is_empty(void) {
    if (SP == MAX_DATA_STACK_HEIGHT) {
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


/* Begin P-Machine */
// instruction format from file is {OP LEVEL M} space-separated.
typedef struct {
   int op;
   int l;
   int m;
} instruction;

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

// LIT
int LIT(int zero, int M) {
    if (BP == 0) {
        stack_push(M);
    } else {
        SP -= 1;
        stack[SP] = M;
    }
}

// OPR
// M determines operation to perform
int OPT(int zero, int M) {
    switch(M) {
        case 0:
            // RET
            SP = BP + 1;
            BP = stack[SP - 3];
            PC = stack[SP - 4];
            break;
        case 1:
            // NEG
            stack[SP] = -stack[SP];
            break;
        case 2:
            // ADD
            SP += 1;
            stack[SP] = stack[SP] + stack[SP - 1];
            break;
        case 3: 
            // SUB
            SP += 1;
            stack[SP] = stack[SP] - stack[SP - 1];
            break;
        case 4:
            // MUL
            SP += 1;
            stack[SP] = stack[SP] * stack[SP - 1];
            break;
        case 5:
            // DIV
            SP += 1;
            stack[SP] = stack[SP] / stack[SP - 1];
            break;
        case 6:
            // ODD
            // @TODO: I cannot make sense of this pseudocode. 
            // stack[sp] = stack[sp] mod 2) or ord(odd(stack[sp]))
            break;
        case 7:
            // MOD
            SP += 1;
            stack[SP] = stack[SP] % stack[SP - 1];
            break;
        case 8:
            // EQL
            SP += 1;
            stack[SP] = stack[SP] == stack[SP - 1];
            break;
        case 9:
            // NEQ
            SP += 1;
            stack[SP] = stack[SP] != stack[SP - 1];
            break;
        case 10:
            // LSS
            SP += 1;
            stack[SP] = stack[SP] < stack[SP - 1];
            break;
        case 11:
            // LEQ
            SP += 1;
            stack[SP] = stack[SP] <= stack[SP - 1];
            break;
        case 12:
            // GTR
            SP += 1;
            stack[SP] = stack[SP] > stack[SP - 1];
            break;
        case 13:
            // GEQ
            SP += 1;
            stack[SP] = stack[SP] >= stack[SP - 1];
            break;
    }
}

int LOD(int L, int M) {
    if (base(L, BP) == 0) {
        GP += 1;
        stack[GP] = stack[base(L, BP) + M];
    } else {
        SP -= 1;
        stack[SP] = stack[base(L, BP) - M];
    }
}

int STO(int L, int M) {
    if (base(L, BP) == 0) {
        stack[base(L, BP) + M] = stack[GP];
        GP -= 1;
    } else {
        stack[base(L, BP) - M] = stack[SP];
        SP += 1;
    }
}

int CAL(int L, int M) {
    if (SP - 4 <= GP) {
        printf("Stack overflow error!");
        return 0;
    }
    stack[SP - 1] = 0;
    stack[SP - 2] = base(L, BP);
    stack[SP - 3] = BP;
    stack[SP - 4] = PC;
    BP = SP - 1;
    PC = M;
}

int INC(int zero, int M) {
    if (SP - M <= GP) {
        printf("Stack overflow error!");
        return 0;
    }
    if (BP == 0) {
        GP = GP + M;
    } else {
        SP = SP - M;
    }
}

int JMP(int zero, int M) {
    PC = M;
}

int JPC(int zero, int M) {
    if (stack[SP] == 0) {
        PC = M;
    }
    SP += 1;
}

int SIO(int zero, int M) {
    switch(M) {
        case 1:
            printf("%d", stack[SP]);
            SP += 1;
            break;
        case 2:
            SP -= 1;
            // read user input into stack
            printf("Enter integer to push to stack: ");
            scanf("%d", &stack[SP]);
            break;
        case 3:
            halt = 0;
    }
}

// ISA
int do_operation(instruction * instr) {
    int M = instr -> m;
    int L = instr -> l;
    int operation = instr -> op;
    switch(operation) {
        case 01:
            // LIT, 0, M
            // Push literal M onto data or stack
            LIT(0, M);
            break;
        case 02:
            // OPR, 0, M
            // Operation to be performed on the data at the top of the stack
            OPR(0, M);
            break;
        case 03:
            // LOD, L, M
            // Load value to top of stack from the stack location at offset M from L lexicographical levels down
            LOD(L, M);
            break;
        case 04: 
            // STO, L, M
            // Store value at top of stack in the stack location at offset M from L lexi levels down
            STO(L, M);
            break;
        case 05:
            // CAL, L, M
            // Call procedure at code index M
            // Generate new activation record and pc <- M
            CAL(L, M);
            break;
        case 06:
            // INC 0, M
            // Allocate M locals, increment SP by M. 
            // First 3 are Static Link (SL), Dynamic Link (DL), and return address (RA)
            INC(0, M);
            break;
        case 07:
            // JMP 0, M
            // Jump to instruction M
            JMP(0, M);
            break;
        case 8:
            // JPC 0, M
            // Jump to instruction M if top stack element == 0
            JPC(0, M);
            break;
        case 9:
            // SIO 0, 1
            // Write top of stack to screen
            // pop? peek? who knows
            SIO(0, 1);
            break;
        case 10:
            // SIO 0, 2
            // Read in input from user and store at top of stack
            SIO(0, 2);
            break;
        case 11: 
            // SIO 0, 3
            // End of program: halt condition
            SIO(0, 3);
            break;
    }
}

// @TODO write a control method

int main(void) {
    printf("AAA");
}

