/**
 * 2020-2 computer architecture 2nd project
 * RISC-V simulator
 * 201911290 Dohyun Kim
 * 2020.12.18.
 *
 * debug mode: ./riscv_sim ./runme.hex 0
 *   run mode: ./riscv_sim ./runme.hex 1
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

 //clock cycles
long long cycles = 0;

// registers
long long int regs[32];

// program counter
unsigned long pc = 0;

// memory
#define INST_MEM_SIZE 32*1024
#define DATA_MEM_SIZE 32*1024
long inst_mem[INST_MEM_SIZE]; //instruction memory
unsigned long long data_mem[DATA_MEM_SIZE]; //data memory

//misc. function
int init(char* filename);

void fetch();//fetch an instruction from a instruction memory
void decode();//decode the instruction and read data from register file
void exe();//perform the appropriate operation 
void mem();//access the data memory
void wb();//write result of arithmetic operation or data read from the data memory if required

void print_cycles();
void print_reg();
void print_pc();

// for my global variables
long inst;
long opcode, rs1, rs2, rd;
long imm;
char oper[20]; // only for debugging

int main(int ac, char *av[])
{

	if (ac < 3)
	{
		printf("./riscv_sim filename mode\n");
		return -1;
	}


	char done = FALSE;
	if (init(av[1]) != 0)
		return -1;

	while (!done)
	{

		fetch();
		decode();
		exe();
		mem();
		wb();


		cycles++;    //increase clock cycle

		//if debug mode, print clock cycle, pc, reg 
		if (*av[2] == '0') {
			print_cycles();  //print clock cycles
//			printf("%s rd=%d rs1=%d rs2=%d imm=%d\n", oper, rd, rs1, rs2, imm);

			print_pc();		 //print pc
			print_reg();	 //print registers
		}

		// check the exit condition, do not delete!! 
		if (regs[9] == 10)  //if value in $t1 is 10, finish the simulation
			done = TRUE;
	}

	if (*av[2] == '1')
	{
		print_cycles();  //print clock cycles
		print_pc();		 //print pc
		print_reg();	 //print registers
	}

	return 0;
}


/* initialize all datapat elements
//fill the instruction and data memory
//reset the registers
*/
int init(char* filename)
{
	FILE* fp = fopen(filename, "r");
	int i;
	long inst;

	if (fp == NULL)
	{
		fprintf(stderr, "Error opening file.\n");
		return -1;
	}

	/* fill instruction memory */
	i = 0;
	while (fscanf(fp, "%lx", &inst) == 1)
	{
		inst_mem[i++] = inst;
	}


	/*reset the registers*/
	for (i = 0; i < 32; i++)
	{
		regs[i] = 0;
	}

	/*reset pc*/
	pc = 0;
	/*reset clock cycles*/
	cycles = 0;
	return 0;
}

void print_cycles()
{
	printf("---------------------------------------------------\n");

	printf("Clock cycles = %lld\n", cycles);
}

void print_pc()
{
	printf("PC	   = %ld\n\n", pc * 4);
}

void print_reg()
{
	printf("x0   = %d\n", regs[0]);
	printf("x1   = %d\n", regs[1]);
	printf("x2   = %d\n", regs[2]);
	printf("x3   = %d\n", regs[3]);
	printf("x4   = %d\n", regs[4]);
	printf("x5   = %d\n", regs[5]);
	printf("x6   = %d\n", regs[6]);
	printf("x7   = %d\n", regs[7]);
	printf("x8   = %d\n", regs[8]);
	printf("x9   = %d\n", regs[9]);
	printf("x10  = %d\n", regs[10]);
	printf("x11  = %d\n", regs[11]);
	printf("x12  = %d\n", regs[12]);
	printf("x13  = %d\n", regs[13]);
	printf("x14  = %d\n", regs[14]);
	printf("x15  = %d\n", regs[15]);
	printf("x16  = %d\n", regs[16]);
	printf("x17  = %d\n", regs[17]);
	printf("x18  = %d\n", regs[18]);
	printf("x19  = %d\n", regs[19]);
	printf("x20  = %d\n", regs[20]);
	printf("x21  = %d\n", regs[21]);
	printf("x22  = %d\n", regs[22]);
	printf("x23  = %d\n", regs[23]);
	printf("x24  = %d\n", regs[24]);
	printf("x25  = %d\n", regs[25]);
	printf("x26  = %d\n", regs[26]);
	printf("x27  = %d\n", regs[27]);
	printf("x28  = %d\n", regs[28]);
	printf("x29  = %d\n", regs[29]);
	printf("x30  = %d\n", regs[30]);
	printf("x31  = %d\n", regs[31]);
	printf("\n");
}


void fetch() {
	inst = inst_mem[pc];
	if (inst == 0) exit(0);
	pc++;
}

void decode() {

	unsigned long tmp;

	imm = 0;
	opcode = inst & 0x7f;
	rs1 = (inst & 0xf8000) >> 15;
	rs2 = (inst & 0x1f00000) >> 20;
	rd = (inst & 0xf80) >> 7;

	switch (opcode) {
	case 0x33: // add R-type
		strcpy(oper, "add");

		break;

	case 0x13: // addi I-type
		strcpy(oper, "addi");

		imm = (inst & 0xfff00000) >> 20;
		if (imm & 0x800) imm |= 0xfffff000;

		break;

	case 0x6f: // jal UJ-type
		strcpy(oper, "jal");

		tmp = inst >> 12;
		imm += (tmp & 0x7fe00) >> 8; // [10:1]
		imm += (tmp & 0x100) << 3; // [11]
		imm += (tmp & 0xff) << 12; // [19:12]
		imm += (tmp & 0x80000) << 1; // [20]

		if (imm & 0x100000)
			imm |= 0xffe00000;

		break;

	case 0x67: // jalr I-type
		strcpy(oper, "jalr");

		imm = (inst & 0xfff00000) >> 20;

		break;

	case 0x63: // beq SB-type
		strcpy(oper, "beq");

		imm = (inst & 0xf00) >> 7; // [4:1]
		imm += (inst & 0x7e000000) >> 20; // [10:5]
		imm += (inst & 0x80) << 4; // [11]
		imm += (inst & 0x80000000) >> 19; // [12]

		if (imm & 0x1000) imm |= 0xFFFFE000;

		break;

	case 0x03: // ld I-type
		strcpy(oper, "ld");

		imm = (inst & 0xfff00000) >> 20;

		break;

	case 0x23: // sd S-type
		strcpy(oper, "sd");

		imm = (inst & 0xf80) >> 7;
		imm += (inst & 0xfe000000) >> 20;

		break;
	}

	regs[0] = 0;
}

void exe() {

	switch (opcode) {
	case 0x33: // add R-type

		regs[rd] = regs[rs1] + regs[rs2];

		break;

	case 0x13: // addi I-type

		regs[rd] = regs[rs1] + imm;

		break;

	case 0x6f: // jal UJ-type

		regs[rd] = pc * 4;
		pc = pc - 1 + imm / sizeof(long);

		break;

	case 0x67: // jalr I-type

		regs[rd] = pc * 4;
		pc = (regs[rs1] + imm) / sizeof(long);

		break;

	case 0x63: // beq SB-type

		if (regs[rs1] == regs[rs2]) pc = pc - 1 + imm / sizeof(long);

		break;

	case 0x03: // ld I-type

		regs[rd] = data_mem[(regs[rs1] + imm) / sizeof(long)];

		break;

	case 0x23: // sd S-type

		data_mem[(regs[rs1] + imm) / sizeof(long)] = regs[rs2];

		break;
	}
}

void mem() {

}

void wb() {

}

