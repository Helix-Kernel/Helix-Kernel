#ifndef PMM_HPP
#define PMM_HPP 1

#include <cstdint>
#include <cstddef>

namespace mm::pmm {

void initialise();

void* alloc(size_t n);
void free(void* p, size_t n);
size_t total_memory();
size_t total_used();
size_t errors_count();

}

#endif
