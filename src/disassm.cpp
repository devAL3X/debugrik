#include "disassm.hpp"

#include <capstone/capstone.h>
#include <iostream>

Disassm::Disassm() {
    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) {
        std::cerr << "ERROR: Failed to initialize Capstone engine!" << std::endl;
        return;
    }
}

void Disassm::print_disassembly(uint8_t *code, uint64_t code_size, uint64_t address){
    cs_insn *insn;

    size_t count = cs_disasm(handle, code, code_size, address, 0, &insn);
    if (count > 0) {
        for (size_t i = 0; i < count; i++) {
            std::cout << "0x" << std::hex << insn[i].address << ":\t";
            for (size_t j = 0; j < insn[i].size; j++) {
                std::cout << std::hex << static_cast<int>(insn[i].bytes[j]) << " ";
            }
            std::cout << "\t" << insn[i].mnemonic << "\t" << insn[i].op_str << std::endl;
        }
        cs_free(insn, count);
    } else {
        std::cerr << "ERROR: Failed to disassemble the code!" << std::endl;
    }
}

int Disassm::onaddr_instr_sz(uint64_t addr) {
    
    
    return 0;
}