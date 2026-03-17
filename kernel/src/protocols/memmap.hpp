#ifndef MEMMAP_HPP
#define MEMMAP_HPP 1

#include <cstddef>
#include <limine.h>

class MemMap {
private:
	size_t entry_count;
	limine_memmap_entry** entries;

	size_t total_size_bytes;

public:
	MemMap();
	~MemMap();

	size_t get_num_entries();
	limine_memmap_entry* get_entry(size_t idx);

	size_t get_total_memory();
};

extern MemMap gMemMap;

#endif
