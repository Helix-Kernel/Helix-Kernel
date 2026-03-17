#include "gdt.hpp"

alignas(16) static arch::x86_64::gdt::GDT  s_gdt;
alignas(16) static arch::x86_64::gdt::GDTR s_gdtr;
alignas(16) static arch::x86_64::gdt::TSS  s_tss;

static uint8_t kernel_stack[8192];

static constexpr arch::x86_64::gdt::GDT default_gdt = {
    {0, 0, 0, 0x00, 0x00, 0},
    {0, 0, 0, 0x9A, 0xA0, 0},
    {0, 0, 0, 0x92, 0xA0, 0},
    {0, 0, 0, 0xF2, 0xA0, 0},
    {0, 0, 0, 0xFA, 0xA0, 0},
    {0, 0, 0, 0x89, 0x00, 0, 0, 0}
};

static void install_tss() {
    const uint64_t base = reinterpret_cast<uint64_t>(&s_tss);
    const uint16_t limit = sizeof(arch::x86_64::gdt::TSS) - 1;

    s_gdt.tss.limit_low = limit & 0xFFFF;
    s_gdt.tss.base_low = base & 0xFFFF;
    s_gdt.tss.base_mid1 = (base >> 16) & 0xFF;
    s_gdt.tss.access = 0x89;
    s_gdt.tss.granularity = 0x00;
    s_gdt.tss.base_mid2 = (base >> 24) & 0xFF;
    s_gdt.tss.base_high = (base >> 32) & 0xFFFFFFFF;
    s_gdt.tss.reserved = 0;
}

void arch::x86_64::gdt::initialise() {
    s_gdt = default_gdt;

    s_tss = {};
    s_tss.rsp[0] = reinterpret_cast<uint64_t>(kernel_stack + sizeof(kernel_stack));
    s_tss.ist[0] = s_tss.rsp[0];
    s_tss.iopb_offset = sizeof(TSS);

    install_tss();

    s_gdtr.limit = sizeof(GDT) - 1;
    s_gdtr.base = reinterpret_cast<uint64_t>(&s_gdt);

    gdt_load(&s_gdtr);
    tss_load();
}

void arch::x86_64::gdt::reload() {
    gdt_load(&s_gdtr);
    tss_load();
}

void arch::x86_64::gdt::update_stack(uint64_t rsp) {
    if (rsp == 0)
        asm volatile("mov %%rsp, %0" : "=r"(rsp));
    s_tss.rsp[0] = s_tss.ist[0] = rsp;
}

void arch::x86_64::gdt::set_rsp(int level, uint64_t rsp) {
    if (level >= 0 && level <= 2)
        s_tss.rsp[level] = rsp;
}

void arch::x86_64::gdt::set_ist(int index, uint64_t rsp) {
    if (index >= 1 && index <= 7)
        s_tss.ist[index - 1] = rsp;
}

void* arch::x86_64::gdt::get_base() { return &s_gdt; }
void* arch::x86_64::gdt::get_tss_base() { return &s_tss; }
