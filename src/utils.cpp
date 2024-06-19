#include "utils.hpp"
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>


_NORETURN panic(std::string msg) { panic(msg.c_str()); }

_NORETURN panic(char *buf) {
    printf("%s %s\n", HEADER_PANIC, buf);
    exit(-1);
}

void o_log(const char *msg, const char *arg) {
    printf("%s %s <v: %s>\n", HEADER_DEBUGGER, msg, arg);
}

bool read_process_memory(pid_t pid, uint64_t address, uint8_t *buffer,
                         size_t size) {
    size_t bytesRead = 0;
    while (bytesRead < size) {
        long word = ptrace(PTRACE_PEEKDATA, pid, address + bytesRead, nullptr);
        if (word == -1) {
            return false;
        }
        memcpy(buffer + bytesRead, &word, sizeof(word));
        bytesRead += sizeof(word);
    }
    return true;
}

void dump(void *st_addr, uint64_t *buf, int k) {
    for(int i = 0; i < k; i++) {
        if(i % 2 == 0)
            std::cout << std::endl << std::hex << st_addr << ": ";
        std::cout << std::hex << std::setw(16) << std::setfill('0') <<  buf[i] << " ";
    }

    std::cout << std::endl;
}