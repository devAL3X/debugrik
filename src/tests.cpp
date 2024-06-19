#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <unistd.h>
#include <iomanip>
#include <sstream>

extern "C" {
    // Mocking the ptrace system call
    long ptrace(enum __ptrace_request, ...);
}

using ::testing::Return;
using ::testing::_;

// Mock class for ptrace
class MockPtrace {
public:
    MOCK_METHOD(long, ptrace, (enum __ptrace_request request, pid_t pid, void *addr, void *data));
};

MockPtrace* mock_ptrace = nullptr;

// Override the ptrace call with the mock
long ptrace(enum __ptrace_request request, pid_t pid, void *addr, void *data) {
    puts("Hey, great work");
    return mock_ptrace->ptrace(request, pid, addr, data);
}

// The function under test
bool read_process_memory(pid_t pid, uint64_t address, uint8_t *buffer, size_t size) {
    size_t bytesRead = 0;
    while (bytesRead < size) {
        u_int64_t word = ptrace(PTRACE_PEEKDATA, pid, reinterpret_cast<void*>(address + bytesRead), nullptr);
        if (word == -1) {
            return false;
        }

        memcpy(buffer + bytesRead, &word, sizeof(word));
        bytesRead += sizeof(word);
    }

    return true;
}

// The function under test
void dump(void *st_addr, uint64_t *buf, int k) {
    std::cout << std::hex << std::setfill('0');
    for(int i = 0; i < k; i++) {
        if(i % 2 == 0)
            std::cout << std::endl << reinterpret_cast<uint64_t>(st_addr) + i * sizeof(uint64_t) << ": ";
        std::cout << std::setw(16) << buf[i] << " ";
    }
    std::cout << std::endl;
}

class MemoryUtilsTest : public ::testing::Test {
protected:
    void SetUp() override {
        mock_ptrace = new MockPtrace();
    }

    void TearDown() override {
        delete mock_ptrace;
        mock_ptrace = nullptr;
    }
};

TEST_F(MemoryUtilsTest, ReadProcessMemory_Success) {
    uint8_t buffer[8];
    memset(buffer, 0, sizeof(buffer));
    pid_t test_pid = 123;
    uint64_t test_address = 0x1234;

    EXPECT_CALL(*mock_ptrace, ptrace(PTRACE_PEEKDATA, test_pid, reinterpret_cast<void*>(test_address), nullptr))
        .WillOnce(Return(0xdeadbeefdeadbeef));

    //EXPECT_CALL(*mock_ptrace, ptrace(PTRACE_PEEKDATA, test_pid, reinterpret_cast<void*>(test_address + 8), nullptr))
     //   .WillOnce(Return(0xcafebabe));

    ASSERT_TRUE(read_process_memory(test_pid, test_address, buffer, sizeof(buffer)));

}