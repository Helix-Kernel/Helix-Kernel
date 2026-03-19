#include "panic.hpp"
#include <cstdio>

void panic(const char* __restrict reason) {
	printf("!!! KERNEL PANIC !!!\r\n%s\r\n", reason);

	asm ("cli;hlt");
	while (1) { asm("hlt"); }
}

void fatal_no_panic(const char* __restrict reason) {
	printf("!!! KERNEL FATAL ERROR !!!\r\n%s\r\n", reason);
}

void warning(const char* __restrict reason) {
	printf("!!! WARNING !!!\r\n%s\r\n", reason);
}
