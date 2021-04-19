#include <stdio.h>

int main() {
	int tmp, inst, imm = 0;

	inst = 0x80000000; // 1000 0000 0000 0000 0000 0000 0000 0000
	inst = 0xffffffff; // 1111 1111 1111 1111 1111 1111 1111 1111
	inst = 0x01180463; // txt 에 있는거

	imm = (inst & 0xf00) >> 7; // [4:1]
	printf("%x\n", imm);
	imm += (inst & 0x7e000000) >> 20; // [10:5]
	printf("%x\n", imm);
	imm += (inst & 0x80) << 4; // [11]
	printf("%x\n", imm);
	imm += (inst & 0x80000000) >> 19; // [12]

	printf("%x\n", imm);

	return 0;
}