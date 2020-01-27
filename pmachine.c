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
int code[MAX_CODE_LENGTH];	// pretty sure this needs to be an instruction pointer array		|| obsolete as well.

// output mega strings (might go 2-d later on)
char ** output_one;
char * output_two = {"\t\tpc\tbp\tsp\tregisters\nInitial values\t"};

void update_output_one(char * OP, int R, int L, int M, int i);
void update_output_two();
char * dynamic_strcat(char * base, char * added);
// strangely: if halt is false then program should halt, else continue
int halt = 0;

// registers
int PC = 0;
int IR = 0;

int REG[REG_FILE_LENGTH];

/* Begin Stack */
/* ===========================================*/
// these are also registers
int BP = 1; // base pointer
int SP = 0; // stack pointer

int stack_is_empty(void) {
    if (SP == BP) {
        return 1;
    } else {
        return 0;
    }
}

int stack_is_full(void) {
    if (SP == MAX_DATA_STACK_HEIGHT) {
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
        SP -= 1;
        return tmp;
    } else {
        printf("Stack is empty!");
    }
}

int stack_push(int data) {
    if (!stack_is_full()) {
        SP += 1;
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
int do_operation(instruction instr) {
    int M = instr.m;
    int L = instr.l;
    int operation = instr.op;
    int R = instr.reg;

	PC++;

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
		printf("fahjfhl;asfd\n");
		output_two = dynamic_strcat(output_two, output_one[PC-1]);
		update_output_two();
}

// write a control method and handle input

int read_in(FILE * fp, instruction * text) {
	int lines_of_text = 0;
	while (fscanf(fp, "%d %d %d %d", &text[lines_of_text].op, &text[lines_of_text].reg, &text[lines_of_text].l, &text[lines_of_text].m) != EOF) {
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

void update_output_one(char * OP, int R, int L, int M, int i) {
	char tmp[3];
	printf("gamer word %d\n", i);
	output_one[i] = dynamic_strcat(output_one[i], OP);
	printf("ni-\n");
	sprintf(tmp, "\t%d", R);
	output_one[i] = dynamic_strcat(output_one[i], tmp);
	printf("sad days\n");
	sprintf(tmp, " %d", L);
	output_one[i] = dynamic_strcat(output_one[i], tmp);

	sprintf(tmp, " %d", M);
	output_one[i] = dynamic_strcat(output_one[i], tmp);

	return;
}

void update_output_two() {
	char tmp[3];
	int i;

	// update the registers
	sprintf(tmp, "%d\t", PC);
	output_two = dynamic_strcat(output_two, tmp);

	sprintf(tmp, "%d\t", BP);
	output_two = dynamic_strcat(output_two, tmp);

	sprintf(tmp, "%d\t", SP);
	output_two = dynamic_strcat(output_two, tmp);

	for (i = 0; i < REG_FILE_LENGTH - 1; i++) {
		sprintf(tmp, "%d ", REG[i]);
		output_two = dynamic_strcat(output_two, tmp);
	}
	sprintf(tmp, "%d\n", REG[REG_FILE_LENGTH - 1]);
	output_two = dynamic_strcat(output_two, tmp);

	// update the stack
	output_two = dynamic_strcat(output_two, "Stack:");
	for (i = 0; i < SP; i++) {
		sprintf(tmp, " %d", stack[i]);
		output_two = dynamic_strcat(output_two, tmp);		
	}
	output_two = dynamic_strcat(output_two, "\n\n");		

	return;
}

char * ins_to_string(instruction ins, int i) {
	char * tmp = (char *)calloc(4, sizeof(char));

	// determine the OP
	switch(ins.op) {
		case 1:
			tmp = "lit";
			break;
		case 2:
			tmp = "rtn";
			break;
		case 3:
			tmp = "lod";
			break;
		case 4:
			tmp = "sto";
			break;
		case 5:
			tmp = "cal";
			break;
		case 6:
			tmp = "inc";
			break;
		case 7:
			tmp = "jmp";
			break;
		case 8:
			tmp = "jpc";
			break;
		case 9:
		case 10:
		case 11:
			tmp = "sio";
			break;
		case 12:
			tmp = "neg";
			break;
		case 13:
			tmp = "add";
			break;
		case 14:
			tmp = "sub";
			break;
		case 15:
			tmp = "mul";
			break;
		case 16:
			tmp = "div";
			break;
		case 17:
			tmp = "odd";
			break;
		case 18:
			tmp = "mod";
			break;
		case 19:
			tmp = "eql";
			break;
		case 20:
			tmp = "neq";
			break;
		case 21:
			tmp = "lss";
			break;
		case 22:
			tmp = "leq";
			break;
		case 23:
			tmp = "gtr";
			break;
		case 24:
			tmp = "geq";
			break;
		default:
			printf("literally not possible");
}
	// function the rest
		update_output_one(tmp, ins.reg, ins.l, ins.m, i);

	return output_one[i];
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

	// initialize output one double array
	output_one = (char **)calloc(lines_of_text, sizeof(char *));
	// loop to create output one
	for (i = 0; i < lines_of_text; i++) {
		printf("%d\n", i);
		printf("yeet\n");
		output_one[i] = (char *)calloc(1, sizeof(char));
		printf("yah\n");
		sprintf(line_index, "%d\t", i);
		printf("yote\n");
		output_one[i] = dynamic_strcat(output_one[i], line_index);
		printf("schleet\n");
		output_one[i] = ins_to_string(text[i], i);
	}

	// use said text array for the mega-while loop
	update_output_two();
	while(!halt) {
		do_operation(text[PC]);
	}

	printf("pass the boof\n");

	for (i = 0; i < lines_of_text; i++) {
		printf("%s\n", output_one[i]);
	}
	printf("%s\n", output_two);
	// write the strings to the output file		
	fclose(fp);
	printf("close the sesame\n");
	fp = fopen("output.txt", "w");
	
	fprintf(fp, "Line\tOP\tR L M\n");
	for (i = 0; i < lines_of_text; i++) {
		fprintf(fp, "%s\n", output_one[i]);
	}
	fprintf(fp, "%s\n", output_two);

	fclose(fp);
	free(text);
	free(output_one);
	return 0;
}

