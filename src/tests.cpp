#include "utils.hpp"
#include <cstdarg>
#include <cstring>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iomanip>
#include <iostream>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

using testing::_;

class MockPtrace {
  public:
    MOCK_METHOD(long, ptrace,
                (enum __ptrace_request request, pid_t pid, void *addr,
                 void *data));
};
MockPtrace *mock_ptrace = nullptr;

/*
    Хукаем, чтобы вместо настоящей функции вызывалась наша обертка
*/
extern "C" {
long ptrace(enum __ptrace_request req, ...) {
    va_list args;
    va_start(args, req);

    pid_t pid = va_arg(args, pid_t);
    void *addr = va_arg(args, void *);
    void *data = va_arg(args, void *);

    va_end(args);
    return mock_ptrace->ptrace(req, pid, addr, data);
}
}

class AllUtilsTest : public ::testing::Test {
  protected:
    void SetUp() {
        mock_ptrace = new MockPtrace();
        errno = 0;
    }

    void TearDown() { delete mock_ptrace; }
};

/*
    Здесь начинается логика тестов
*/

#define TEST_SAMPLE_VALUE 0xcafebabecafebabe
#define TEST_SAMPLE_VADDR 0x1234
#define TEST_PID 0xdead

TEST_F(AllUtilsTest, ReadProcessMemory_8bytesSuccess) {
    uint8_t buffer[8];

    EXPECT_CALL(*mock_ptrace, ptrace(PTRACE_PEEKDATA, TEST_PID, _, _))
        .Times(1)
        .WillRepeatedly(testing::Return(TEST_SAMPLE_VALUE));

    ASSERT_TRUE(read_process_memory(TEST_PID, TEST_SAMPLE_VADDR, buffer,
                                    sizeof(buffer)));
    EXPECT_EQ(*((u_int64_t *)&buffer[0]), TEST_SAMPLE_VALUE);
}

TEST_F(AllUtilsTest, ReadProcessMemory_24bytesSuccess) {
    uint8_t buffer[24];

    EXPECT_CALL(*mock_ptrace, ptrace(PTRACE_PEEKDATA, TEST_PID, _, _))
        .Times(3) // 24 bytes splits into 4 calls by 8 bytes
        .WillRepeatedly(testing::Return(TEST_SAMPLE_VALUE));

    ASSERT_TRUE(read_process_memory(TEST_PID, TEST_SAMPLE_VADDR, buffer,
                                    sizeof(buffer)));

    for (int i = 0; i < 3; i++)
        EXPECT_EQ(((u_int64_t *)buffer)[i], TEST_SAMPLE_VALUE);
}

TEST_F(AllUtilsTest, ReadProcessMemory_ptrace_failure) {
    uint8_t buffer[8];

    EXPECT_CALL(*mock_ptrace, ptrace(PTRACE_PEEKDATA, TEST_PID, _, _))
        .Times(1)
        .WillRepeatedly(testing::Return(-1)); // simulate error from ptrace

    ASSERT_FALSE(read_process_memory(TEST_PID, TEST_SAMPLE_VADDR, buffer,
                                     sizeof(buffer)));
}

TEST(DumpTest, DumpEmptyBuffer) {
    uint64_t buffer[0];
    testing::internal::CaptureStdout();
    dump(nullptr, buffer, 0);
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ(output, "\n");
}

TEST(DumpTest, DumpSingleValue) {
    uint64_t buffer[1] = {0x1234567890ABCDEF};
    testing::internal::CaptureStdout();
    dump(nullptr, buffer, 1);
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ(output, "\n0: 1234567890abcdef \n");
}

TEST(DumpTest, DumpMultipleValues) {
    uint64_t buffer[4] = {0x1234567890ABCDEF, 0x1122334455667788,
                          0xAABBCCDDEEFF0011, 0x9988776655443322};
    testing::internal::CaptureStdout();
    dump(nullptr, buffer, 4);
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ(output, "\n0: 1234567890abcdef 1122334455667788 \n0: "
                      "aabbccddeeff0011 9988776655443322 \n");
}