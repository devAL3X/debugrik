#ifndef DISASSM_H
#define DISASSM_H

#include <cstdint>
#include <capstone/capstone.h>

class Disassm {
  public:
    Disassm();
    void print_disassembly(uint8_t *start, uint64_t size, uint64_t address);

    int onaddr_instr_sz(uint64_t addr);

  private:
    csh handle;
};

#endif