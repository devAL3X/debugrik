#ifndef DISASSM_H
#define DISASSM_H

#include <cstdint>
#include <capstone/capstone.h>

#define MAX_INSTR_SIZE  16*2

class Disassm {
  public:
    Disassm();
    void print_disassembly(uint8_t *start, uint64_t size, uint64_t address);

    uint64_t *next_instr_addr(uint8_t *code, uint64_t code_size, uint64_t address);

  private:
    csh handle;
};

#endif