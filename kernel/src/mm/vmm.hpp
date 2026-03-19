#ifndef VMM_HPP
#define VMM_HPP 1

#include <cstdint>
#include <cstddef>

// Page table entry flags
#define PAGE_PRESENT        0x001   // Page is present in memory
#define PAGE_RW             0x002   // Read/write
#define PAGE_USER           0x004   // User-mode accessible
#define PAGE_PWT            0x008   // Page-level write-through
#define PAGE_PCD            0x010   // Page-level cache-disable
#define PAGE_ACCESSED       0x020   // Has been accessed (set by CPU)
#define PAGE_DIRTY          0x040   // Has been written to (set by CPU)
#define PAGE_PAT            0x080   // Page attribute table index
#define PAGE_GLOBAL         0x100   // Global page, not flushed by CR3 reload
#define PAGE_NX             (1ULL << 63)  // No-execute (probably wont use cuz, FEEEEL)

#define PAGE_PS             0x080   // Page size (1 = large page, 2MB/1GB)
#define PAGE_AVAIL          0x200   // Available to the kernel

namespace mm::vmm {

void initialise();

void* alloc(size_t n);
void free(void* p, size_t n);

void* mmap(void* paddr, void* vaddr, size_t n, uint64_t flags);
void munmap(void* vaddr, size_t n);

int copy_to_user(void* data, void* user_memory, size_t n);
int copy_from_user(void* data, void* kernel_memory, size_t n);

uint64_t get_cr3();
void switch_to_new_table(uint64_t cr3);

uint64_t va_to_pa(uint64_t va);
void* va_to_pa(void* va);

uint64_t pa_to_va(uint64_t pa);
void* pa_to_va(void* pa);

}

#endif
