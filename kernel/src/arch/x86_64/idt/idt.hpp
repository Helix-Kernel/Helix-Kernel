#ifndef IDT_HPP
#define IDT_HPP 1

#include <cstdint>

struct exception_frame {
    uint64_t rax, rbx, rcx, rdx, rbp, rsi, rdi;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
    uint64_t exception_vector;
    uint64_t error_code;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;

    uint64_t cr0, cr2, cr3, cr4;
    uint16_t ds, es, fs, gs;
} __attribute__((packed));

typedef struct {
	uint16_t limit;
	uint64_t base;
} __attribute__((packed)) idtr_t;

typedef struct {
	uint16_t isr_offset_low;
	uint16_t gdt_selector;
	uint8_t ist;
	uint8_t flags; /* usually 0x8E */
	uint16_t isr_offset_middle;
	uint32_t isr_offset_high;
	uint32_t always_zero;
} __attribute__((packed)) idt_entry_t;

typedef struct {
	idt_entry_t entries[256];
} __attribute__((packed)) idt_t;

extern "C" void idt_load(const idtr_t* idtr);

typedef void (*irq_handler_t)(uint8_t irq, void* context);
typedef void* irq_handle_t;

namespace arch::x86_64::idt {

void initialise();
void register_isr(uint8_t vector, void* isr_handler, bool user_interrupt);

irq_handle_t register_irq(uint8_t irq, irq_handler_t handler); // use protection key to ensure other drivers cant manage this IRQ
void remove_irq(uint8_t irq, irq_handle_t protection_key);

void unmask_irq(uint8_t irq, irq_handle_t protection_key);
void mask_irq(uint8_t irq, irq_handle_t protection_key);

}

#endif
