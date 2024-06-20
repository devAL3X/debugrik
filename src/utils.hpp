#ifndef UTILS_H
#define UTILS_H

#include <cstdint>
#include <string>

#define HEADER_PANIC "panic:"
#define HEADER_DEBUGGER "dbg:"

#define _NORETURN void

/**
 * Logs a message with an optional argument.
 *
 * @param msg The message to be logged.
 * @param arg An optional argument to be included in the log message.
 */
void o_log(const char *msg, const char *arg);

/**
 * @brief Causes the program to terminate abnormally with a panic message.
 *
 * This function is used to indicate a critical error condition that cannot be
 * recovered from. It prints the specified error message and terminates the
 * program.
 *
 * @param msg The panic message to be displayed.
 * @return This function does not return a value.
 */
_NORETURN panic(std::string msg);

/**
 * @brief Causes the program to terminate abnormally.
 *
 * This function is used to indicate a critical error or an unrecoverable
 * condition that requires the program to stop execution immediately. It prints
 * the specified error message to the standard error stream and terminates the
 * program.
 *
 * @param buf A pointer to a null-terminated string containing the error
 * message.
 * @return This function does not return a value.
 */
_NORETURN panic(char *buf);

/**
 * Reads the memory of a process with the specified process ID (pid) at the
 * given address.
 *
 * @param pid The process ID of the target process.
 * @param address The memory address to read from.
 * @param buffer A pointer to the buffer where the read data will be stored.
 * @param size The number of bytes to read.
 * @return `true` if the memory was successfully read, `false` otherwise.
 */
bool read_process_memory(pid_t pid, uint64_t address, uint8_t *buffer,
                         size_t size);

/**
 * Dumps the contents of a memory region to an array.
 *
 * @param st_addr The starting address of the memory region.
 * @param buf     The buffer to store the dumped contents.
 * @param k       The number of elements to dump.
 */
void dump(void *st_addr, uint64_t *buf, int k);

#endif