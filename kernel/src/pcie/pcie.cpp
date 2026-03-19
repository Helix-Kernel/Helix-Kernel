#include "pcie.hpp"
#include <arch/x86_64/port_io.hpp>

// this is the only file that doesn't follow the rest of the workspace style rules, because we have two namespaces

// legacy PCI
namespace pcie::legacy {

uint32_t pci_read(uint8_t b, uint8_t d, uint8_t f, uint8_t offset) {
    uint32_t lbus  = (uint32_t)b;
    uint32_t lslot = (uint32_t)d;
    uint32_t lfunc = (uint32_t)f;

    uint32_t addr = (1 << 31)
                  | (lbus  << 16)
                  | (lslot << 11)
                  | (lfunc <<  8)
                  | (offset & 0xFC);

    outl(0xCF8, addr);
    return (inl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF;
}

void pci_write(uint8_t b, uint8_t d, uint8_t f, uint32_t data, uint8_t bytes, uint8_t offset) {
    uint32_t lbus  = (uint32_t)b;
    uint32_t lslot = (uint32_t)d;
    uint32_t lfunc = (uint32_t)f;

    uint32_t addr = (1 << 31)
                  | (lbus  << 16)
                  | (lslot << 11)
                  | (lfunc <<  8)
                  | (offset & 0xFC);

    outl(0xCF8, addr);

    switch (bytes) {
        case 1: outb(0xCFC + (offset & 3), (uint8_t)data);  break;
        case 2: outw(0xCFC + (offset & 2), (uint16_t)data); break;
        case 4: outl(0xCFC, data);                           break;
    }
}

}

// PCIe
namespace pcie {

}
