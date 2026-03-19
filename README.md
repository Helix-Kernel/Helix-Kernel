# HelixKernel V2

## To-Do list

### Initial
- [x] Set up kernel entry and command line parsing into `int main(int argc, char** argv)`
- [x] Port mintsuki/flanterm and mpaland/printf
- [ ] **x86_64 Specific**
- [ ] **Kernel**
- [ ] **Finale**

### x86_64 Specific
- [x] Prepare a physical memory manager
- [x] Prepare a virtual memory manager
- [x] Write an allocator, probably port liballoc
- [x] Write a GDT
- [x] Write an IDT
- [x] Write an IRQ manager
- [x] Port or write an ACPI kernel driver
- [ ] APIC to replace the PIC
- [ ] HPET timer
- [ ] APIC timer
- [ ] Syscalls
- [ ]  **Scheduler**

### Scheduler
- [ ] Multitasking
- [ ] Multithreading
- [ ] Multitasking on more than one CPU
- [ ] User-space tasks

### Kernel
- [ ] Drivers
  - [ ] PCIe
    - [ ] PCIe hot-plug using ACPI
  - [ ] Disk drivers
    - [ ] AHCI
    - [ ] NVMe
  - [ ] Partition Management
    - [ ] MBR
    - [ ] GPT
  - [ ] Network cards
    - [ ] E1000 - all variants
    - [ ] RTL8139 - all variants
  - [ ] probably audio?
    - [ ] USB
    - [ ] XHCI
  - [ ] Input drivers
    - [ ] PS/2 Input
      - [ ] PS/2 Keyboard
      - [ ] PS/2 Mouse
    - [ ] USB Input
      - [ ] USB Keyboard
      - [ ] USB Mouse
- [ ] Subsystems
  - [ ] RamFS
  - [ ] DevFS
  - [ ] ProcFS
  - [ ] VFS
  - [ ] Network stack
    - [ ] Ethernet
    - [ ] ARP
    - [ ] IPv4
    - [ ] ICMP
    - [ ] TCP
    - [ ] UDP
- [ ] Filesystems
  - [ ] FAT
    - [ ] FAT32
    - [ ] FAT16
    - [ ] FAT12
  - [ ] EXT
    - [ ] EXT2
    - [ ] EXT3
    - [ ] EXT4
- [ ] Syscalls
  - [ ] HlAPI **( 0x80000000 - 0xFFFFFFFFFFFFFFFF )**
  - [ ] POSIX **( 0x0 - 0x7FFFFFFF )**
- [ ] Executables
  - [ ] ELF
    - [ ] Relocation
    - [ ] Dynamic interpreter loading
  - [ ] PE32+
    - [ ] Relocation
    - [ ] DLL loading

### Finale
- [ ] User-space drivers
- [ ] Port mlibc
- [ ] Porting software
- [ ] Writing software
- [ ] DONE!1!
