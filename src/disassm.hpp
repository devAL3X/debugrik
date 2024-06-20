#ifndef DISASSM_H
#define DISASSM_H

#include <capstone/capstone.h>
#include <cstdint>

#define MAX_INSTR_SIZE 16 * 2

/**
 * @brief The Disassm class provides functionality for disassembling binary
 * code.
 */
class Disassm {
  public:
    Disassm();
    /**
     * Prints the disassembly of the given memory range.
     *
     * @param start The pointer to the start of the memory range.
     * @param size The size of the memory range.
     * @param address The address of the memory range.
     */
    void print_disassembly(uint8_t *start, uint64_t size, uint64_t address);

    /**
     * @brief Finds the address of the next instruction in the given code.
     *
     * @param code The binary code to search for the next instruction.
     * @param code_size The size of the binary code.
     * @param address The current address in the code.
     * @return The address of the next instruction.
     */
    uint64_t *next_instr_addr(uint8_t *code, uint64_t code_size,
                              uint64_t address);

  private:
    csh handle;
};

#endif