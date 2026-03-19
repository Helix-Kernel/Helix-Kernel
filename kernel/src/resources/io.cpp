#include "io.hpp"
#include <cstdlib>
#include <cstdio>
#include <vector>

vector<io_mapping> io_mappings;

io_mapping* map_io(uint16_t base, uint16_t len, char* owner) {
    io_mapping mapping = { base, len, owner };
    io_mappings.push_back(mapping);
    return &io_mappings.at(io_mappings.size() - 1);
}

void unmap_io(io_mapping* mapping) {
    for (size_t i = 0; i < io_mappings.size(); i++) {
        if (&io_mappings[i] == mapping) {
            io_mappings.erase(io_mappings.begin() + i);
            return;
        }
    }
}

void print_io_mappings() {
    for (size_t i = 0; i < io_mappings.size(); i++) {
        io_mapping& m = io_mappings.at(i);
        printf("%s:\t0x%04X\t-\t0x%04X\r\n", m.owner, m.base_port, m.base_port + m.len_ports);
    }
}
