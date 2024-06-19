#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <cstdint>

#define HEADER_PANIC "panic:"
#define HEADER_DEBUGGER "dbg:"

#define _NORETURN void

void o_log(const char *msg, const char *arg);

_NORETURN panic(std::string msg);

_NORETURN panic(char *buf);
bool read_process_memory(pid_t pid, uint64_t address, uint8_t *buffer,
                         size_t size);
#endif