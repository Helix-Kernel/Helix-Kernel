#ifndef GDT_HPP
#define GDT_HPP 1

#include <cstdint>

namespace arch::x86_64::gdt {

constexpr uint16_t SEL_KCODE = 0x08;
constexpr uint16_t SEL_KDATA = 0x10;
constexpr uint16_t SEL_UDATA = 0x18 | 3;
constexpr uint16_t SEL_UCODE = 0x20 | 3;
constexpr uint16_t SEL_TSS   = 0x28;

struct [[gnu::packed]] GDTEntry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_mid;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
};

struct [[gnu::packed]] TSSEntry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_mid1;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_mid2;
    uint32_t base_high;
    uint32_t reserved;
};

struct [[gnu::packed]] GDT {
    GDTEntry null;
    GDTEntry kcode;
    GDTEntry kdata;
    GDTEntry udata;
    GDTEntry ucode;
    TSSEntry tss;
};

struct [[gnu::packed]] GDTR {
    uint16_t limit;
    uint64_t base;
};

struct [[gnu::packed]] TSS {
    uint32_t reserved0;
    uint64_t rsp[3];
    uint64_t reserved1;
    uint64_t ist[7];
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iopb_offset;
};

void initialise();
void reload();
void update_stack(uint64_t rsp = 0);
void set_rsp(int level, uint64_t rsp);
void set_ist(int index, uint64_t rsp);
void* get_base();
void* get_tss_base();

}

extern "C" void gdt_load(const arch::x86_64::gdt::GDTR* gdtr);
extern "C" void tss_load();

#endif
