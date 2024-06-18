#ifndef UTILS_H
#define UTILS_H

#include <string>

#define HEADER_PANIC    "panic:"
#define HEADER_DEBUGGER "dbg:"

#define _NORETURN void

void o_log(const char *msg, const char *arg);

_NORETURN panic(std::string msg);

_NORETURN panic(char *buf);

#endif