#include <cstring>

#include "vmm.hpp"
#include "pmm.hpp"

uint64_t kernel_cr3;
uint64_t current_cr3;

void mm::vmm::initialise() {
	asm volatile(
	    ".intel_syntax noprefix\n"
	    "mov %0, cr3\n"
	    ".att_syntax prefix"
	    : "=r"(kernel_cr3)
	    :
	    : "memory"
	);

	current_cr3 = kernel_cr3;
}

void* mm::vmm::alloc(size_t n) {
	return pa_to_va(mm::pmm::alloc(n));
}

void mm::vmm::free(void* p, size_t n) {
	mm::pmm::free(va_to_pa(p), n);
}

#include <cstring>
#include "vmm.hpp"
#include "pmm.hpp"

#define PAGE_PRESENT    (1ULL << 0)
#define PAGE_WRITE      (1ULL << 1)
#define PAGE_USER       (1ULL << 2)
#define PAGE_NX         (1ULL << 63)
#define PAGE_ADDR_MASK  0x000FFFFFFFFFF000ULL
#define PAGE_SIZE       0x1000ULL

#define PML4_IDX(va)  (((va) >> 39) & 0x1FF)
#define PDPT_IDX(va)  (((va) >> 30) & 0x1FF)
#define PD_IDX(va)    (((va) >> 21) & 0x1FF)
#define PT_IDX(va)    (((va) >> 12) & 0x1FF)

static inline void invlpg(void* vaddr) {
    asm volatile("invlpg (%0)" :: "r"(vaddr) : "memory");
}

static uint64_t* get_or_create_table(uint64_t* parent, size_t idx, uint64_t flags) {
    if (!(parent[idx] & PAGE_PRESENT)) {
        void* page = mm::pmm::alloc(1);
        if (!page) return nullptr;

        void* vpage = mm::vmm::pa_to_va(page);
        memset(vpage, 0, PAGE_SIZE);

        parent[idx] = (uint64_t)page | PAGE_PRESENT | PAGE_WRITE | (flags & PAGE_USER);
    }
    return (uint64_t*)mm::vmm::pa_to_va((void*)(parent[idx] & PAGE_ADDR_MASK));
}

void* mm::vmm::mmap(void* paddr, void* vaddr, size_t n, uint64_t flags) {
    // Addresses below the canonical user boundary are user mappings
    if ((uint64_t)vaddr < 0x800000000000) flags |= PAGE_USER;

    uint64_t va = (uint64_t)vaddr;
    uint64_t pa = (uint64_t)paddr;

    uint64_t* pml4 = (uint64_t*)pa_to_va((void*)current_cr3);

    for (size_t i = 0; i < n; i++, va += PAGE_SIZE, pa += PAGE_SIZE) {
        uint64_t* pdpt = get_or_create_table(pml4, PML4_IDX(va), flags);
        if (!pdpt) return nullptr;

        uint64_t* pd = get_or_create_table(pdpt, PDPT_IDX(va), flags);
        if (!pd) return nullptr;

        uint64_t* pt = get_or_create_table(pd, PD_IDX(va), flags);
        if (!pt) return nullptr;

        pt[PT_IDX(va)] = pa | flags | PAGE_PRESENT;
        invlpg((void*)va);
    }

    return vaddr;
}

void mm::vmm::munmap(void* vaddr, size_t n) {
    uint64_t va = (uint64_t)vaddr;
    uint64_t end = va + n;

    while (va < end) {
        uint64_t* pml4 = (uint64_t*)pa_to_va((void*)current_cr3);
        uint64_t* pml4e = &pml4[PML4_IDX(va)];
        if (!(*pml4e & PAGE_PRESENT)) { va += PAGE_SIZE; continue; }

        uint64_t* pdpt = (uint64_t*)pa_to_va((void*)(*pml4e & PAGE_ADDR_MASK));
        uint64_t* pdpte = &pdpt[PDPT_IDX(va)];
        if (!(*pdpte & PAGE_PRESENT)) { va += PAGE_SIZE; continue; }

        uint64_t* pd = (uint64_t*)pa_to_va((void*)(*pdpte & PAGE_ADDR_MASK));
        uint64_t* pde = &pd[PD_IDX(va)];
        if (!(*pde & PAGE_PRESENT)) { va += PAGE_SIZE; continue; }

        uint64_t* pt = (uint64_t*)pa_to_va((void*)(*pde & PAGE_ADDR_MASK));
        pt[PT_IDX(va)] = 0;
        invlpg((void*)va);

        va += PAGE_SIZE;
    }
}

int mm::vmm::copy_to_user(void* data, void* user_memory, size_t n) {
	if (!data) return -1;
	if (!user_memory) return -1;

	memcpy(data, user_memory, n);

	return 0;
}

int mm::vmm::copy_from_user(void* data, void* kernel_memory, size_t n) {
	if (!data) return -1;
	if (!kernel_memory) return -1;

	memcpy(kernel_memory, data, n);

	return 0;
}

uint64_t mm::vmm::get_cr3() {
	return kernel_cr3;
}

void mm::vmm::switch_to_new_table(uint64_t cr3) {
	if (!cr3) cr3 = kernel_cr3;

	current_cr3 = cr3;

	asm volatile(
	    ".intel_syntax noprefix\n"
	    "mov cr3, rax\n"
	    ".att_syntax prefix"
	    :
	    : "a"(cr3)
	    : "memory"
	);
}

uint64_t mm::vmm::va_to_pa(uint64_t va) {
	if (va <= 0x800000000000) return va;
	return va - 0xFFFF800000000000;
}

void* mm::vmm::va_to_pa(void* va) {
	if ((uint64_t)va <= 0x800000000000) return va;
	return (void*)((uint64_t)va - 0xFFFF800000000000);
}

uint64_t mm::vmm::pa_to_va(uint64_t pa) {
	if (pa >= 0xFFFF800000000000) return pa;
	return pa + 0xFFFF800000000000;
}

void* mm::vmm::pa_to_va(void* pa) {
	if ((uint64_t)pa >= 0xFFFF800000000000) return pa;
	return (void*)((uint64_t)pa + 0xFFFF800000000000);
}
