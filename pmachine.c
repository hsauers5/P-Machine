#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DATA_STACK_HEIGHT 40
#define MAX_CODE_LENGTH 200
#define MAX_LEXI_LEVEL 3
#define REG_FILE_LENGTH 8

// stack, init to 0
int stack[MAX_DATA_STACK_HEIGHT];

int lex[MAX_LEXI_LEVEL];
int code[MAX_CODE_LENGTH];	// pretty sure this needs to be an instruction pointer array

// output mega strings (might go 2-d later on)
char * output_one = {"Line\tOP\tL\tM\n"};
char * output_two = {"\t\t\tgp\tpc\tbp\tsp\tdata\t\t\tstack\nInitial values\t\n"};

// strangely: if halt is false then program should halt, else continue
int halt = -1;

// registers
int PC = 0;
int IR = 0;
int GP = -1;

int REG[REG_FILE_LENGTH];

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
typedef struct instruction {
    int op;
    int reg;
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
int LIT(int R, int zero, int M) {
    /*
    if (BP == 0) {
        stack_push(M);
    } else {
        SP -= 1;
        stack[SP] = M;
    }
    */
    REG[R] = M;
}

// MATH
// OP determines operation to perform
int MATH(int OP, int R, int L, int M) {
    // i = R, j=L, R=REG, k=M
    switch(OP) {
        case 12:
            // NEG
            // stack[SP] = -stack[SP];
            REG[R] = -REG[L];
            break;
        case 13:
            // ADD
            /* SP += 1;
            stack[SP] = stack[SP] + stack[SP - 1]; */
            REG[R] = REG[L] + REG[M];
            break;
        case 14: 
            // SUB
            /* 
            SP += 1;
            stack[SP] = stack[SP] - stack[SP - 1];
            */
            REG[R] = REG[L] - REG[M];
            break;
        case 15:
            // MUL
            /*
            SP += 1;
            stack[SP] = stack[SP] * stack[SP - 1];
            */
            REG[R] = REG[L] * REG[M];
            break;
        case 16:
            // DIV
            /*
            SP += 1;
            stack[SP] = stack[SP] / stack[SP - 1];
            */
            REG[R] = REG[L] / REG[M];
            break;
        case 17:
            // ODD
            // @TODO_done: I cannot make sense of this pseudocode. 
            // stack[sp] = stack[sp] mod 2) or ord(odd(stack[sp]))
          	// stack[SP] = stack[SP] % 2;
            REG[R] = REG[R] % 2;
            break;
        case 18:
            // MOD
            /*
            SP += 1;
            stack[SP] = stack[SP] % stack[SP - 1];
            */
            REG[R] = REG[L] % REG[M];
            break;
        case 19:
            // EQL
            /*
            SP += 1;
            stack[SP] = stack[SP] == stack[SP - 1];
            */
            REG[R] = REG[L] == REG[M];
            break;
        case 20:
            // NEQ
            /*
            SP += 1;
            stack[SP] = stack[SP] != stack[SP - 1];
            */
            REG[R] = REG[L] != REG[M];
            break;
        case 21:
            // LSS
            /*
            SP += 1;
            stack[SP] = stack[SP] < stack[SP - 1];
            */
            REG[R] = REG[L] < REG[M];
            break;
        case 22:
            // LEQ
            /*
            SP += 1;
            stack[SP] = stack[SP] <= stack[SP - 1];
            */
            REG[R] = REG[L] <= REG[M];
            break;
        case 23:
            // GTR
            /*
            SP += 1;
            stack[SP] = stack[SP] > stack[SP - 1];
            */
            REG[R] = REG[L] > REG[M];
            break;
        case 24:
            // GEQ
            /*
            SP += 1;
            stack[SP] = stack[SP] >= stack[SP - 1];
            */
            REG[R] = REG[L] >= REG[M];
            break;
    }
}

int RTN(int zero1, int zero2, int zero3) {
    SP = BP - 1;
    BP = stack[SP + 3];
    PC = stack[SP + 4];
}

int LOD(int R, int L, int M) {
    /*
    if (base(L, BP) == 0) {
        GP += 1;
        stack[GP] = stack[base(L, BP) + M];
    } else {
        SP -= 1;
        stack[SP] = stack[base(L, BP) - M];
    }
    */
    REG[R] = stack[base(L, BP) + M];
}

int STO(int R, int L, int M) {
    /*
    if (base(L, BP) == 0) {
        stack[base(L, BP) + M] = stack[GP];
        GP -= 1;
    } else {
        stack[base(L, BP) - M] = stack[SP];
        SP += 1;
    }
    */
    stack[base(L, BP) + M] = REG[R];
}

int CAL(int zero, int L, int M) {
    /*
    if (SP - 4 <= GP) {
        printf("Stack overflow error!");
        return 0;
    }
    */
    stack[SP + 1] = 0;
    stack[SP + 2] = base(L, BP);
    stack[SP + 3] = BP;
    stack[SP + 4] = PC;
    BP = SP + 1;
    PC = M;
}

int INC(int zero1, int zero2, int M) {
    /*
    if (SP - M <= GP) {
        printf("Stack overflow error!");
        return 0;
    }
    if (BP == 0) {
        GP = GP + M;
    } else {
        SP = SP - M;
    }
    */
    SP += M;
}

int JMP(int zero1, int zero2, int M) {
    PC = M;
}

int JPC(int R, int zero, int M) {
    if (stack[SP] == 0) {
        PC = M;
    }
    // SP += 1;
}

int SIO(int R, int zero, int M) {
    switch(M) {
        case 1:
            printf("%d", REG[SP]);
            // SP += 1;
            break;
        case 2:
            SP -= 1;
            // read user input into stack
            printf("Enter integer to push to stack: ");
            scanf("%d", &REG[SP]);
            break;
        case 3:
            halt = 1;
    }
}

// ISA
int do_operation(instruction * instr) {
    int M = instr -> m;
    int L = instr -> l;
    int operation = instr -> op;
    int R = instr -> reg;
    switch(operation) {
        case 01:
            // LIT, R, 0, M
            // Push literal M onto data or stack
            LIT(R, 0, M);
            break;
        case 02:
            // RTN, 0, 0, 0
            // Operation to be performed on the data at the top of the stack
            RTN(0, 0, 0);
            break;
        case 03:
            // LOD, R, L, M
            // Load value to top of stack from the stack location at offset M from L lexicographical levels down
            LOD(R, L, M);
            break;
        case 04: 
            // STO, R, L, M
            // Store value at top of stack in the stack location at offset M from L lexi levels down
            STO(R, L, M);
            break;
        case 05:
            // CAL, 0, L, M
            // Call procedure at code index M
            // Generate new activation record and pc <- M
            CAL(0, L, M);
            break;
        case 06:
            // INC 0, 0, M
            // Allocate M locals, increment SP by M. 
            // First 3 are Static Link (SL), Dynamic Link (DL), and return address (RA)
            INC(0, 0, M);
            break;
        case 07:
            // JMP 0, 0, M
            // Jump to instruction M
            JMP(0, 0, M);
            break;
        case 8:
            // JPC, R, 0, M
            // Jump to instruction M if top stack element == 0
            JPC(R, 0, M);
            break;
        case 9:
            // SIO, R, 0, 1
            // Write top of stack to screen
            // pop? peek? who knows
            SIO(R, 0, 1);
            break;
        case 10:
            // SIO, R, 0, 2
            // Read in input from user and store at top of stack
            SIO(R, 0, 2);
            break;
        case 11: 
            // SIO, R, 0, 3
            // End of program: halt condition
            SIO(R, 0, 3);
            break;
        default:
            if (operation >= 12) {
                MATH(operation, R, L, M);
            }
            break;
    }
}

// write a control method and handle input

int read_in(FILE * fp, instruction * text) {
	int lines_of_text = 0;
	while (fscanf(fp, "%d %d %d", &text[lines_of_text].op, &text[lines_of_text].l, &text[lines_of_text].m) != EOF) {
		lines_of_text++;
	}
	
	return lines_of_text;
}

char * dynamic_strcat(char * base, char * added) {
	// resize the base String
	char * conjoined = (char *)malloc(strlen(base) + strlen(added) + 1);

	if (conjoined == NULL) {
		printf("Error: failed to create char array.\n");
		return NULL;
	}

	// strcat and go
	strcpy(conjoined, base);

	return strcat(conjoined, added);
}

int main(void) {
    // init stack
    for (int i = 0; i < MAX_DATA_STACK_HEIGHT; i++) {
        stack[i] = 0;
    }
    
    // init REG
    for (int i = 0; i < REG_FILE_LENGTH; i++) {
        REG[i] = 0;
    }

	FILE *fp = fopen("input.txt", "r");
	instruction * text = (instruction *)calloc(MAX_CODE_LENGTH, sizeof(instruction));
	char line_index[4];
	int lines_of_text;
	int i;

	if (fp == NULL) {
		printf("Error: Could not locate file.\n");
		exit(-1);
	}

	// record number of operations while reading from input.txt
	lines_of_text = read_in(fp, text);
/*
	// use said text array for the mega-while loop
	sprintf(line_index, "%d\t", i);
	for (i = 0; i < lines_of_text; i++) {
		output_one = dynamic_strcat(output_one, line_index);
		do_operation(text[i]);
	}
*/
	// write the strings to the output file		
	fclose(fp);
	fp = fopen("output.txt", "w");
	fprintf(fp, "%s\n%s", output_one, output_two);

	fclose(fp);
	free(text);
	return 0;
}

