#include "idt.hpp"
#include <cstdio>
#include <cstdlib>

alignas(16) static idt_t idt_table;
alignas(16) static idtr_t idtr;

void load_idt() {
	idt_load(&idtr);
}

bool idt_set_vectors[256] = {false};

struct irq_entry {
	irq_handle_t protection_key;
	uint8_t irq;
	irq_handler_t handler;
	bool used;
	bool masked;
} irq_table[16] = {nullptr, 0, nullptr, false, true};

const char* exception_names[] = {
	"#DE", "#DB", "#NMI", "#BP", "#OF", "#BR",
	"#UD", "#NM", "#DF", "N/A", "#TS", "#NP",
	"#SS", "#GP", "#PF", "N/A", "#MF", "#AC",
	"#MC", "#XM", "#VE", "#CP", "N/A", "N/A",
	"N/A", "N/A", "N/A", "N/A", "N/A", "N/A",
	"N/A", "N/A",
};

extern void (*irq_stub_table[16])(void*);
extern "C" uint64_t exception_stub_table[];

void arch::x86_64::idt::initialise() {
	for (int i = 0; i < 0x20; i++) {
		register_isr(i, (void*)exception_stub_table[i], false);
	}
	
	for (int i = 0x20; i <= 0x2F; i++) {
		register_isr(i, (void*)irq_stub_table[i - 0x20], false);
	}
	
	idtr.limit = sizeof(idt_table) - 1;
	idtr.base = (uint64_t)&idt_table;
	load_idt();
}

void arch::x86_64::idt::register_isr(uint8_t vector, void* isr_handler, bool user_interrupt) {
	idt_entry_t* e = &idt_table.entries[vector];
	e->isr_offset_low = ((uint64_t)isr_handler & 0xFFFF);
	e->gdt_selector = 0x08;
	e->ist = 1;
	switch (user_interrupt) {
		case true:
			e->flags = 0xEE;
			break;
		case false:
			e->flags = 0x8E;
			break;
		default:
			e->flags = 0x8E;
			break;
	}
	e->isr_offset_middle = ((uint64_t)isr_handler >> 16) & 0xFFFF;
	e->isr_offset_high = ((uint64_t)isr_handler >> 32) & 0xFFFFFFFF;
	e->always_zero = 0;
	idt_set_vectors[vector] = true;
}

irq_handle_t arch::x86_64::idt::register_irq(uint8_t irq, irq_handler_t handler) {
	if (irq >= 16) return nullptr;
	if (!handler) return nullptr;
	if (irq_table[irq].used) return nullptr;
	else {
		irq_table[irq].used = true;
		irq_table[irq].irq = irq;
		irq_table[irq].handler = handler;
		irq_table[irq].protection_key = malloc(1);
		irq_table[irq].masked = true;
	}
	return irq_table[irq].protection_key;
}

void arch::x86_64::idt::remove_irq(uint8_t irq, irq_handle_t protection_key) {
	if (irq >= 16) return;
	if (!protection_key) return;
	if (!irq_table[irq].used) return;
	else {
		if (irq_table[irq].protection_key == protection_key) {
			irq_table[irq].used = false;
			irq_table[irq].irq = 0;
			irq_table[irq].handler = nullptr;
			free(irq_table[irq].protection_key);
			irq_table[irq].protection_key = nullptr;
			irq_table[irq].masked = true;
		} else return;
	}
}

void arch::x86_64::idt::unmask_irq(uint8_t irq, irq_handle_t protection_key) {
	if (irq >= 16) return;
	if (irq_table[irq].protection_key != protection_key) return;
	irq_table[irq].masked = false;
}

void arch::x86_64::idt::mask_irq(uint8_t irq, irq_handle_t protection_key) {
	if (irq >= 16) return;
	if (irq_table[irq].protection_key != protection_key) return;
	irq_table[irq].masked = true;
}

#define IRQ_STUB(num) \
extern "C" void irq_stub_handler_##num(void* ctx) { \
	if (!irq_table[num].used) return; \
	if (irq_table[num].masked) return; \
	irq_table[num].handler(num, ctx); \
}

IRQ_STUB(0)
IRQ_STUB(1)
IRQ_STUB(2)
IRQ_STUB(3)
IRQ_STUB(4)
IRQ_STUB(5)
IRQ_STUB(6)
IRQ_STUB(7)
IRQ_STUB(8)
IRQ_STUB(9)
IRQ_STUB(10)
IRQ_STUB(11)
IRQ_STUB(12)
IRQ_STUB(13)
IRQ_STUB(14)
IRQ_STUB(15)

void (*irq_stub_table[16])(void*) = {
	irq_stub_handler_0,
	irq_stub_handler_1,
	irq_stub_handler_2,
	irq_stub_handler_3,
	irq_stub_handler_4,
	irq_stub_handler_5,
	irq_stub_handler_6,
	irq_stub_handler_7,
	irq_stub_handler_8,
	irq_stub_handler_9,
	irq_stub_handler_10,
	irq_stub_handler_11,
	irq_stub_handler_12,
	irq_stub_handler_13,
	irq_stub_handler_14,
	irq_stub_handler_15,
};

extern "C" void exception_handler(exception_frame* frame) {
	printf("!!! KERNEL PANIC !!!\r\n");

	uint64_t cr0, cr2, cr3, cr4, ds, es, fs, gs;

	asm volatile("mov %%cr0, %0" : "=r"(cr0));
    asm volatile("mov %%cr2, %0" : "=r"(cr2));
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    
    asm volatile("mov %%ds, %0" : "=r"(ds));
    asm volatile("mov %%es, %0" : "=r"(es));
    asm volatile("mov %%fs, %0" : "=r"(fs));
    asm volatile("mov %%gs, %0" : "=r"(gs));

    uint8_t vec = frame->exception_vector;
    uint64_t err = frame->error_code;
    
    printf("\n\033[91m========== EXCEPTION! ==========\033[0m\n");
    printf("check_exception v=%02x e=%04llx i=0 cpl=%d IP=%04llx:%016llx pc=%016llx CR2=%016llx\n",
           vec, err, (uint32_t)(frame->cs & 3), frame->cs, frame->rip, frame->rip, cr2);
    
    printf("RAX= %016llx RBX= %016llx RCX= %016llx RDX= %016llx\n",
           frame->rax, frame->rbx, frame->rcx, frame->rdx);
    printf("RSI= %016llx RDI= %016llx RBP= %016llx RSP= %016llx\n",
           frame->rsi, frame->rdi, frame->rbp, frame->rsp);
    printf("R8=  %016llx R9=  %016llx R10= %016llx R11= %016llx\n",
           frame->r8, frame->r9, frame->r10, frame->r11);
    printf("R12= %016llx R13= %016llx R14= %016llx R15= %016llx\n",
           frame->r12, frame->r13, frame->r14, frame->r15);
    
    printf("RIP= %016llx RFL= %08llx [", frame->rip, frame->rflags);
    printf("%c", (frame->rflags & (1 << 11)) ? 'O' : '-');
    printf("%c", (frame->rflags & (1 << 10)) ? 'D' : '-');
    printf("%c", (frame->rflags & (1 << 9))  ? 'I' : '-');
    printf("%c", (frame->rflags & (1 << 7))  ? 'S' : '-');
    printf("%c", (frame->rflags & (1 << 6))  ? 'Z' : '-');
    printf("%c", (frame->rflags & (1 << 4))  ? 'A' : '-');
    printf("%c", (frame->rflags & (1 << 2))  ? 'P' : '-');
    printf("%c", (frame->rflags & (1 << 0))  ? 'C' : '-');
    printf("] CPL=%d\n", (uint32_t)(frame->cs & 3));
    
    printf("ES=  %04x DS=  %04x SS=  %04llx CS=  %04llx FS=  %04x GS=  %04x\n",
           es, ds, frame->ss, frame->cs, fs, gs);
    
    printf("CR0= %08llx CR2= %016llx CR3= %016llx CR4= %08llx\n",
           cr0, cr2, cr3, cr4);
    
    printf("Exception: %s (vector=%d, error=%llx)\n", 
           exception_names[vec], vec, err);
}
