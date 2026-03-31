#include "stdio.h"
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ({          \
    const typeof( ((type *)0)->member ) *__mptr = (ptr); \
    (type *)( (char *)__mptr - offsetof(type,member) );})

struct some_data
{
    int a;
    char b;
    char c;
    int d;
};
/*
 * Memory layout
 * [a [4 byte] b[1byte] c[1byte][2byte] d[4byte]] 
 * CPUs generally do not read memory one byte at a time. Instead, they read in "words" (typically 4 or 8 bytes depending on the architecture).
 * After c, just 2 bytes remaining so variable d(4byte) will follow with 2 bytes gap. 
*/
struct some_data data;

int main(int argc, char *argv[])
{
    data.a = 10;
    data.b = 'b';
    data.c = 'c';
    data.d = 11;
    struct some_data *data1 = container_of(&data.c, struct some_data, c); 
    /*
     * After preprocessing, this line will be expand to the following by Macro
	struct some_data *data1 = ({ const typeof( ((struct some_data *)0)->c ) *__mptr = (&data.c);
				 (struct some_data *)( (char *)__mptr - ((size_t) &((struct some_data *)0)->c) );});
    
		1) ((struct some_data *)0)->c: This is dereferencing a NULL pointer and access to c variable and cause to segment fault in 'runtime'.
		2) typeof() is the compiler extionsion run in compile time usually use for type-generic in c, typeof(((struct some_data *)0)->c) will be int, this will \
		in compile time so we will not actually dereference to NULL pointer in run time as we said above in (1).
		3) const typeof( ((struct some_data *)0)->c ) *__mptr = (&data.c); --> const int *__mptr = (&data.c). 
			3.1) const is the promise that you will change the value of mptr pointer (discard warning if the data.c is the constant)
			3.2) This code will check that the name and the type you passed of pointer are the same, if not (const int* = *(char)), this trigger a warning.
		4) (size_t) &((struct some_data *)0)->c): This is the expansion of offsetof macro.
			4.1) Compiler will treat is like an action get the address of member when the address of struct is 0. This will return an offset of member from struct. 
			4.2) The calculation will return an address (32 or 64 bit based on architecture), the (size_t) casting to uint32_t or uint64_t for next calculation.
		5)  (char*)__mptr - (uint64_t) offset will give the base address of the pointer we passed because __mptr now is the address we passed into macro.
		6)  (struct some_data*) is just a typecasting to the struct we want to get.
    */
    printf ("From data1: %d\n", data1->d);
}

