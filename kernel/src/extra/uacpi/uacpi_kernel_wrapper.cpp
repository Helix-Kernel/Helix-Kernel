#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include <uacpi/types.h>
#include <uacpi/kernel_api.h>
#include <protocols/rsdp.hpp>
#include <mm/vmm.hpp>
#include <resources/io.hpp>
#include <arch/x86_64/port_io.hpp>
#include <mutex/mutex.hpp>
#include <arch/x86_64/idt/idt.hpp>
#include <pcie/pcie.hpp>
#include <panic.hpp>

extern "C" {

uacpi_status uacpi_kernel_get_rsdp(uacpi_phys_addr *out_rsdp_address) {
	*out_rsdp_address = (uacpi_phys_addr)rsdp::get_rsdp();

	return UACPI_STATUS_OK;
}

void *uacpi_kernel_map(uacpi_phys_addr addr, uacpi_size len) {
	return mm::vmm::mmap((void*)addr, mm::vmm::pa_to_va((void*)addr), (len + 0xFFF) / 0x1000, PAGE_PRESENT | PAGE_RW);
}

void uacpi_kernel_unmap(void *addr, uacpi_size len) {
	mm::vmm::munmap(addr, (len + 0xFFF) / 0x1000);
}

#ifndef UACPI_FORMATTED_LOGGING
void uacpi_kernel_log(uacpi_log_level, const uacpi_char* s) {
	printf("%s\r", s);
}
#else
void uacpi_kernel_log(uacpi_log_level, const uacpi_char* fmt, ...) {
	va_list va;
	va_start(va, fmt);

	vprintf(fmt, va);

	va_end(va);
}

void uacpi_kernel_vlog(uacpi_log_level, const uacpi_char* fmt, uacpi_va_list va) {
	vprintf(fmt, va);
}
#endif

#ifdef UACPI_KERNEL_INITIALIZATION
uacpi_status uacpi_kernel_initialize(uacpi_init_level current_init_lvl) {
	return UACPI_STATUS_OK;
}
void uacpi_kernel_deinitialize(void) {
	asm ("nop");
}
#endif

struct uacpi_pci_dev {
	uint8_t b, d, f;
};

uacpi_status uacpi_kernel_pci_device_open(uacpi_pci_address address, uacpi_handle *out_handle) {
	uacpi_pci_dev* dev = (uacpi_pci_dev*)malloc(sizeof(uacpi_pci_dev));

	dev->b = address.bus;
	dev->d = address.device;
	dev->f = address.function;

	*out_handle = (uacpi_handle)dev;

	return UACPI_STATUS_OK;
}

void uacpi_kernel_pci_device_close(uacpi_handle handle) {
	free(handle);
}

uacpi_status uacpi_kernel_pci_read8(uacpi_handle device, uacpi_size offset, uacpi_u8 *value) {
	uint8_t b = ((uacpi_pci_dev*)device)->b;
	uint8_t d = ((uacpi_pci_dev*)device)->d;
	uint8_t f = ((uacpi_pci_dev*)device)->f;

	uint32_t val = pcie::legacy::pci_read(b, d, f, offset);

	*value = (uint8_t)val;

	return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_pci_read16(uacpi_handle device, uacpi_size offset, uacpi_u16 *value) {
	uint8_t b = ((uacpi_pci_dev*)device)->b;
	uint8_t d = ((uacpi_pci_dev*)device)->d;
	uint8_t f = ((uacpi_pci_dev*)device)->f;

	uint32_t val = pcie::legacy::pci_read(b, d, f, offset);

	*value = (uint16_t)val;

	return UACPI_STATUS_OK;	
}

uacpi_status uacpi_kernel_pci_read32(uacpi_handle device, uacpi_size offset, uacpi_u32 *value) {
	uint8_t b = ((uacpi_pci_dev*)device)->b;
	uint8_t d = ((uacpi_pci_dev*)device)->d;
	uint8_t f = ((uacpi_pci_dev*)device)->f;

	*value = pcie::legacy::pci_read(b, d, f, offset);

	return UACPI_STATUS_OK;	
}

uacpi_status uacpi_kernel_pci_write8(uacpi_handle device, uacpi_size offset, uacpi_u8 value) {
	uint8_t b = ((uacpi_pci_dev*)device)->b;
	uint8_t d = ((uacpi_pci_dev*)device)->d;
	uint8_t f = ((uacpi_pci_dev*)device)->f;

	pcie::legacy::pci_write(b, d, f, offset, 1, value);

	return UACPI_STATUS_OK;	
}

uacpi_status uacpi_kernel_pci_write16(uacpi_handle device, uacpi_size offset, uacpi_u16 value) {
	uint8_t b = ((uacpi_pci_dev*)device)->b;
	uint8_t d = ((uacpi_pci_dev*)device)->d;
	uint8_t f = ((uacpi_pci_dev*)device)->f;

	pcie::legacy::pci_write(b, d, f, offset, 2, value);

	return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_pci_write32(uacpi_handle device, uacpi_size offset, uacpi_u32 value) {
	uint8_t b = ((uacpi_pci_dev*)device)->b;
	uint8_t d = ((uacpi_pci_dev*)device)->d;
	uint8_t f = ((uacpi_pci_dev*)device)->f;
	
	pcie::legacy::pci_write(b, d, f, offset, 4, value);

	return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_io_map(uacpi_io_addr base, uacpi_size len, uacpi_handle *out_handle) {
	static char uacpi_io_name[] = "uACPI";
	*out_handle = (uacpi_handle)map_io(base, len, uacpi_io_name);

	return UACPI_STATUS_OK;
}

void uacpi_kernel_io_unmap(uacpi_handle handle) {
	unmap_io((io_mapping*)handle);
}

uacpi_status uacpi_kernel_io_read8(uacpi_handle handle, uacpi_size offset, uacpi_u8 *out_value) {
	io_mapping* range = (io_mapping*)handle;

	*out_value = inb(range->base_port + offset);

	return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_io_read16(uacpi_handle handle, uacpi_size offset, uacpi_u16 *out_value) {
	io_mapping* range = (io_mapping*)handle;

	*out_value = inw(range->base_port + offset);

	return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_io_read32(uacpi_handle handle, uacpi_size offset, uacpi_u32 *out_value) {
	io_mapping* range = (io_mapping*)handle;

	*out_value = inl(range->base_port + offset);

	return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_io_write8(uacpi_handle handle, uacpi_size offset, uacpi_u8 in_value) {
	io_mapping* range = (io_mapping*)handle;

	outb(range->base_port + offset, in_value);

	return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_io_write16(uacpi_handle handle, uacpi_size offset, uacpi_u16 in_value) {
	io_mapping* range = (io_mapping*)handle;

	outw(range->base_port + offset, in_value);

	return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_io_write32(uacpi_handle handle, uacpi_size offset, uacpi_u32 in_value) {
	io_mapping* range = (io_mapping*)handle;

	outl(range->base_port + offset, in_value);

	return UACPI_STATUS_OK;
}

void *uacpi_kernel_alloc(uacpi_size size) {
	return malloc(size);
}

#ifdef UACPI_NATIVE_ALLOC_ZEROED
void *uacpi_kernel_alloc_zeroed(uacpi_size size) {
	return calloc(1, size);
}
#endif

#ifndef UACPI_SIZED_FREES
void uacpi_kernel_free(void *mem) {
	free(mem);
}
#else
void uacpi_kernel_free(void *mem, uacpi_size size_hint) {
	free(mem);
}
#endif

uacpi_u64 uacpi_kernel_get_nanoseconds_since_boot(void) {
	return 0;
}

void uacpi_kernel_stall(uacpi_u8 usec) {
	for (int i = 0; i < usec*100000; i++);
}

void uacpi_kernel_sleep(uacpi_u64 msec) {
	for (uacpi_u64 i = 0; i < msec*1000; i++);
}

uacpi_handle uacpi_kernel_create_mutex(void) {
	return (uacpi_handle)create_mutex();
}

void uacpi_kernel_free_mutex(uacpi_handle mutex) {
	destroy_mutex((mutex_t)mutex);
}

struct event {
    volatile bool signaled;
};

uacpi_handle uacpi_kernel_create_event(void) {
    event* e = (event*)malloc(sizeof(event));
    e->signaled = false;
    return e;
}

void uacpi_kernel_free_event(uacpi_handle handle) {
    free(handle);
}

uacpi_thread_id uacpi_kernel_get_thread_id(void) {
	return (uacpi_thread_id)(uintptr_t)1;
}

uacpi_interrupt_state uacpi_kernel_disable_interrupts(void) {
	asm ("cli");

	return 0;
}

void uacpi_kernel_restore_interrupts(uacpi_interrupt_state state) {
	(void)state;

	asm ("sti");
}

uacpi_status uacpi_kernel_acquire_mutex(uacpi_handle mutex, uacpi_u16) {
	mutex_lock((mutex_t)mutex);

	return UACPI_STATUS_OK;
}

void uacpi_kernel_release_mutex(uacpi_handle mutex) {
	mutex_unlock((mutex_t)mutex);
}

uacpi_bool uacpi_kernel_wait_for_event(uacpi_handle handle, uacpi_u16 timeout) {
    event* e = (event*)handle;
    if (!e) return false;

    if (timeout == 0x0000) {
        return e->signaled;
    }

    if (timeout == 0xFFFF) {
        while (!e->signaled) {
            __asm__ __volatile__("pause");
        }
        return true;
    }

    while (!e->signaled) {
        __asm__ __volatile__("pause");
    }

    return true;
}

void uacpi_kernel_signal_event(uacpi_handle handle) {
    event* e = (event*)handle;
    if (e) e->signaled = true;
}

void uacpi_kernel_reset_event(uacpi_handle handle) {
    event* e = (event*)handle;
    if (e) e->signaled = false;
}

uacpi_status uacpi_kernel_handle_firmware_request(uacpi_firmware_request* request) {
    if (request->type == UACPI_FIRMWARE_REQUEST_TYPE_BREAKPOINT) {
    } else if (request->type == UACPI_FIRMWARE_REQUEST_TYPE_FATAL) {
        panic((char*)"UACPI_FIRMWARE_REQUEST_TYPE_FATAL");
    }
    return UACPI_STATUS_OK;
}

struct irq_entry {
    uacpi_interrupt_handler handler;
    uacpi_handle ctx;
};

static irq_entry irq_table[256];

static void irq_trampoline(uint8_t irq, void*) {
    if (irq_table[irq].handler)
        irq_table[irq].handler(irq_table[irq].ctx);
}

uacpi_status uacpi_kernel_install_interrupt_handler(
    uacpi_u32 irq,
    uacpi_interrupt_handler irq_handler,
    uacpi_handle ctx,
    uacpi_handle *out_irq_handle
) {
	irq_table[irq].handler = irq_handler;
	irq_table[irq].ctx = ctx;
	*out_irq_handle = (uacpi_handle)arch::x86_64::idt::register_irq((uint8_t)irq, irq_trampoline);

	return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_uninstall_interrupt_handler(
    uacpi_interrupt_handler,
    uacpi_handle irq_handle
) {
	(void)irq_handle;
	return UACPI_STATUS_INTERNAL_ERROR;
}

uacpi_handle uacpi_kernel_create_spinlock(void) {
	return uacpi_kernel_create_mutex();
}

void uacpi_kernel_free_spinlock(uacpi_handle spinlock) {
	uacpi_kernel_free_mutex(spinlock);
}

uacpi_cpu_flags uacpi_kernel_lock_spinlock(uacpi_handle spinlock) {
	uacpi_kernel_acquire_mutex(spinlock, 0xFFFF);

	return 0;
}

void uacpi_kernel_unlock_spinlock(uacpi_handle spinlock, uacpi_cpu_flags) {
	uacpi_kernel_release_mutex(spinlock);
}

uacpi_status uacpi_kernel_schedule_work(
    uacpi_work_type,
    uacpi_work_handler,
    uacpi_handle ctx
) {
	return UACPI_STATUS_UNIMPLEMENTED;
}

uacpi_status uacpi_kernel_wait_for_work_completion(void) {
	return UACPI_STATUS_OK;
}

}
