#include <cstdio>

#include "pmm.hpp"
#include <protocols/memmap.hpp>
#include <limine.h>

struct block {
	block* next;
};

static inline uintptr_t buddy_of(uintptr_t addr, int order) {
	return addr ^ (1ULL << order);
}

static inline void add_block(block*& head, block* b) {
	b->next = head;
	head = b;
}

static inline block* remove_block(block*& head) {
	block* b = head;
	if (b) head = b->next;
	return b;
}

struct pmm_segment {
	uintptr_t base_ptr;
	size_t size;
	bool fully_used;

	uint64_t pages_used;
	block* free_list[16];
	int max_order;
};

pmm_segment segtab[128];
constexpr int max_segtab = 128;
int num_segtab;

uint64_t total_mem;
uint64_t num_used;
uint64_t num_errors;

void mm::pmm::initialise() {
    // Iterate through all memmap entries
    for (uint64_t i = 0; i < gMemMap.get_num_entries(); i++) {
        limine_memmap_entry* mme = gMemMap.get_entry(i);

        // Only consider usable, ACPI reclaimable, and bootloader reclaimable, thanks limine :>
        if (mme->type != LIMINE_MEMMAP_USABLE &&
            mme->type != LIMINE_MEMMAP_ACPI_RECLAIMABLE &&
            mme->type != LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE)
            continue;

        uintptr_t base = (uintptr_t)mme->base;
        size_t length = mme->length;

        // Skip memory below 1 MiB
        if (base + length <= 0x100000) {
            //printf("Skipping segment entirely below 1 MiB: base=0x%016x, size=%zu\r\n", base, length);
            continue;
        }

        if (base < 0x100000) {
            // Trim the part below 1 MiB
            size_t delta = 0x100000 - base;
            base += delta;
            length -= delta;
            //printf("Trimming segment below 1 MiB: new base=0x%016x, new size=%zu\r\n", base, length);
        }

        if (num_segtab >= max_segtab) {
            //printf("Segment table limit exceeded\r\n");
            break;
        }

        pmm_segment* seg = &segtab[num_segtab];
        seg->base_ptr = base;
        seg->size = length;
        seg->pages_used = 0;

        for (int j = 0; j < 16; j++)
            seg->free_list[j] = nullptr;

        size_t pages = length >> 12;

        int order = 0;
        while ((1ULL << (order + 1)) <= pages) order++;
        seg->max_order = order;

        add_block(seg->free_list[order], (block*)seg->base_ptr);

        //printf("Added PMM segment: base=0x%016x, size=%zu\r\n", base, length);
        num_segtab++;
    }

    // Total memory in pages
    size_t total_size = 0;
    for (int i = 0; i < num_segtab; i++)
        total_size += segtab[i].size;
    total_mem = (total_size + 0xFFF) / 0x1000;

    num_used = 0;
    num_errors = 0;

    //printf("Total memory initialized: %zu bytes\r\n", total_size);
}

void* mm::pmm::alloc(size_t n) {
    size_t pages = (n + 0xFFF) >> 12;

    int order = 0;
    while ((1ULL << order) < pages) order++;

    for (int i = 0; i < num_segtab; i++) {
        pmm_segment* seg = &segtab[i];

        for (int current = order; current <= seg->max_order; current++) {
            if (!seg->free_list[current]) continue;

            block* b = remove_block(seg->free_list[current]);

            while (current > order) {
                current--;

                uintptr_t buddy_addr = (uintptr_t)b + (1ULL << (current + 12));
                add_block(seg->free_list[current], (block*)buddy_addr);
            }

            seg->pages_used += (1ULL << order);
            num_used += (1ULL << order);

            return (void*)b;
        }
    }

    num_errors++;
    return nullptr;
}

void mm::pmm::free(void* p, size_t n) {
    uintptr_t addr = (uintptr_t)p;

    size_t pages = (n + 0xFFF) >> 12;

    int order = 0;
    while ((1ULL << order) < pages) order++;

    for (int i = 0; i < num_segtab; i++) {
        pmm_segment* seg = &segtab[i];

        if (addr < seg->base_ptr || addr >= seg->base_ptr + seg->size)
            continue;

        uintptr_t base = seg->base_ptr;

        while (order < seg->max_order) {
            uintptr_t rel = addr - base;
            uintptr_t buddy_rel = rel ^ (1ULL << (order + 12));
            uintptr_t buddy_addr = base + buddy_rel;

            block** head = &seg->free_list[order];
            block* prev = nullptr;
            block* curr = *head;

            bool found = false;

            while (curr) {
                if ((uintptr_t)curr == buddy_addr) {
                    if (prev) prev->next = curr->next;
                    else *head = curr->next;

                    found = true;
                    break;
                }
                prev = curr;
                curr = curr->next;
            }

            if (!found) break;

            if (buddy_addr < addr) addr = buddy_addr;
            order++;
        }

        add_block(seg->free_list[order], (block*)addr);

        seg->pages_used -= (1ULL << order);
        num_used -= (1ULL << order);

        return;
    }
}

size_t mm::pmm::total_memory() {
	size_t total = 0;
	for (int i = 0; i < num_segtab; i++)
		total += segtab[i].size;
	return total;
}

size_t mm::pmm::total_used() {
	return num_used;
}

size_t mm::pmm::errors_count() {
	return num_errors;
}
