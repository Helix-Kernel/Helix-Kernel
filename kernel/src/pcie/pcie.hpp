#ifndef PCIE_HPP
#define PCIE_HPP 1

#include <cstdint>

struct __attribute__((packed)) pci_config_space {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t revision_id;
    uint8_t prog_if;
    uint8_t subclass;
    uint8_t class_code;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
    uint8_t raw[4080];
};

namespace pcie::legacy {

uint32_t pci_read(uint8_t b, uint8_t d, uint8_t f, uint8_t offset);
void pci_write(uint8_t b, uint8_t d, uint8_t f, uint32_t data, uint8_t bytes, uint8_t offset);

}

namespace pcie {



}

#endif
