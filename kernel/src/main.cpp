#include <cstdio>
#include <cstdlib>

#include <mm/pmm.hpp>
#include <mm/vmm.hpp>
#include <arch/x86_64/gdt/gdt.hpp>
#include <arch/x86_64/idt/idt.hpp>
#include <uacpi/uacpi.h>
#include <resources/io.hpp>

void initialised(const char* component) {
	printf("[ OK ] %s Initialised\r\n", component);
}

int main(int argc, char** argv) {
	(void)argc;(void)argv;

	__cstdio_initialise_terminal();

	mm::pmm::initialise();
	initialised("PMM");

	mm::vmm::initialise();
	initialised("VMM");

	void* ist1_alloc = malloc(65536 + 16);
	uint64_t ist1_top = ((uint64_t)ist1_alloc + 65536) & ~0xFull;
	
	arch::x86_64::gdt::initialise();
	arch::x86_64::gdt::update_stack();
	arch::x86_64::gdt::set_ist(1, ist1_top);
	initialised("GDT");

	arch::x86_64::idt::initialise();
	initialised("IDT");

	uacpi_initialize(
		UACPI_FLAG_BAD_CSUM_FATAL |
		UACPI_FLAG_BAD_TBL_SIGNATURE_FATAL
	);
	initialised("uACPI");

	printf("Dumping IO mappings after ACPI initialisation\r\n");
	print_io_mappings();

	return 0;
}
