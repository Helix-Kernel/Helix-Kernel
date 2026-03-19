#ifndef IO_HPP
#define IO_HPP 1

#include <cstdint>

struct io_mapping {
    uint16_t base_port;
    uint16_t len_ports;
    char* owner;
};

io_mapping* map_io(uint16_t base, uint16_t len, char* owner);
void unmap_io(io_mapping* mapping);

void print_io_mappings();

#endif
