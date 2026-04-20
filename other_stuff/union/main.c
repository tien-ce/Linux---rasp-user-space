#include <stdio.h>
#include <string.h>
struct my_struct {
	int x;
	float f;
	char str[16];
};

union my_union {
	int x;
	float f;
	char str[16];
};

union HWRegister {
	struct {
		unsigned char control;
		unsigned char direction;
		unsigned char parity;
		unsigned char stopbits;
	};
	unsigned int reg;
};
int main(){
	union my_union Onion;
//	struct my_struct Stroct;
//	Onion.f = 16.00;
//	strcpy(Onion.str,"Hello world!");
//	printf("%.2f\n",Onion.f);
	union HWRegister GPIOA;
	GPIOA.reg = 0x21346596;
	printf ("%x\n",GPIOA.direction);
}
