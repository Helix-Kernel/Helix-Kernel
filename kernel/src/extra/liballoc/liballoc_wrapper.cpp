#include <mm/vmm.hpp>

extern "C" int liballoc_lock() {
	asm ("cli");
	return 0;
}

extern "C" int liballoc_unlock() {
	asm ("sti");
	return 0;
}

extern "C" void* liballoc_alloc(int npages) {
	return mm::vmm::alloc(npages);
}

extern "C" int liballoc_free(void* p, int hint) {
	mm::vmm::free(p, hint);

	return 0;
}
