#include "memmap.hpp"
#include <cstdio>
#include <limine.h>

MemMap gMemMap;

__attribute__((section(".limine_requests")))
static volatile limine_memmap_request memmap_request = {
	.id = LIMINE_MEMMAP_REQUEST_ID,
	.revision = 0
};

MemMap::MemMap() {
	if (!memmap_request.response ||
		memmap_request.response->entry_count < 1 ||
		!memmap_request.response->entries ||
		!memmap_request.response->entries[0]) {
		asm ("cli;hlt");
	}

	this->entry_count = memmap_request.response->entry_count;
	this->entries = memmap_request.response->entries;
}

MemMap::~MemMap() {
	// no-op
}

size_t MemMap::get_num_entries() {
	return this->entry_count;
}

limine_memmap_entry* MemMap::get_entry(size_t idx) {
	return this->entries[idx];
}

size_t MemMap::get_total_memory() {
	return this->total_size_bytes;
}
