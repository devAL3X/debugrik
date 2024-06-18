#include "utils.hpp"

_NORETURN panic(std::string msg) { panic(msg.c_str()); }

_NORETURN panic(char *buf) {
    printf("%s %s\n", HEADER_PANIC, buf);
    exit(-1);
}

void o_log(const char *msg, const char *arg) {
    printf("%s %s <v: %s>\n", HEADER_DEBUGGER, msg, arg);
}