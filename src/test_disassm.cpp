#include "disassm.hpp"
#include <gtest/gtest.h>

TEST(DisassmTest, ValidCodeDisassembly) {
    Disassm disassm;
    uint8_t code[] = {0x55, 0x48, 0x89, 0xe5, 0x48, 0x83, 0xec, 0x10,
                      0x48, 0x89, 0x7d, 0xf8, 0x48, 0x8b, 0x45, 0xf8};
    uint64_t code_size = sizeof(code);
    uint64_t address = 0x1000;

    testing::internal::CaptureStdout();
    disassm.print_disassembly(code, code_size, address);
    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_TRUE(output.find("push\trbp") != std::string::npos);
    ASSERT_TRUE(output.find("mov\trbp, rsp") != std::string::npos);
    ASSERT_TRUE(output.find("sub\trsp, 0x10") != std::string::npos);
    ASSERT_TRUE(output.find("mov\trax, qword ptr [rbp - 8]") !=
                std::string::npos);

    ASSERT_TRUE(output.find("Error") == std::string::npos);
}

TEST(DisassmTest, InvalidCodeDisassembly) {
    Disassm disassm;
    uint8_t invalid_code[] = {0x55, 0x48, 0x89, 0xe5};
    uint64_t invalid_code_size = sizeof(invalid_code);
    uint64_t invalid_address = 0x2000;

    testing::internal::CaptureStdout();
    disassm.print_disassembly(invalid_code, invalid_code_size, invalid_address);
    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_FALSE(output.find("Error") != std::string::npos);
}

TEST(DisassmTest, NextInstructionAddressIsNullptr) {
    Disassm disassm;
    uint8_t code[] = {0x55, 0x48, 0x89, 0xe5, 0x48, 0x83, 0xec, 0x20, 0x64,
                      0x48, 0x8b, 0x04, 0x25, 0x28, 0x00, 0x00, 0x00, 0x48};
    uint64_t code_size = sizeof(code);
    uint64_t address = 0x1000;

    uint64_t *next_addr = disassm.next_instr_addr(code, code_size, address);

    ASSERT_EQ(next_addr, nullptr);
}

TEST(DisassmTest, CurrInstructionIsCall) {
    Disassm disassm;

    uint8_t code[] = {0xe8, 0xd8, 0xff, 0xff, 0xff, 0xb8, 0x0, 0x0, 0x0, 0x0, 0xcc};
    uint64_t code_size = sizeof(code);
    uint64_t address = 0x1000;

    uint64_t *next_addr = disassm.next_instr_addr(code, code_size, address);

    ASSERT_NE(next_addr, nullptr);
}
