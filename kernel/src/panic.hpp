#ifndef PANIC_HPP
#define PANIC_HPP 1

void panic(const char* __restrict reason);
void fatal_no_panic(const char* __restrict reason);
void warning(const char* __restrict reason);

#endif
