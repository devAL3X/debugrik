#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "cfg.hpp"
#include "disassm.hpp"
#include "dwarfinfo.hpp"
#include "utils.hpp"

#include <map>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>

#define MAX_BREAKPOINTS 100
#define MAX_XREAD_K 8

#define MSG_SHOULD_BE_RUNNED "target not started"
#define MSG_ALREADY_STARTED "target is already in run"

#define expand_regs(regs)                                                      \
    {{"rdi", regs.rdi}, {"rsi", regs.rsi}, {"rdx", regs.rdx},                  \
     {"rcx", regs.rcx}, {"rax", regs.rax}, {" r8", regs.r8},                   \
     {" r9", regs.r9},  {"r10", regs.r10}, {"r11", regs.r11},                  \
     {"r12", regs.r12}, {"r13", regs.r13}, {"r14", regs.r14},                  \
     {"r15", regs.r15}, {"rbx", regs.rbx}, {"rbp", regs.rbp},                  \
     {"rsp", regs.rsp}, {"rip", regs.rip}, {"efl", regs.eflags}};

#define flatten_regs(regs, rm)                                                 \
    regs.rdi = rm["rdi"];                                                      \
    regs.rsi = rm["rsi"];                                                      \
    regs.rdx = rm["rdx"];                                                      \
    regs.rcx = rm["rcx"];                                                      \
    regs.rax = rm["rax"];                                                      \
    regs.r8 = rm["r8"];                                                        \
    regs.r9 = rm["r9"];                                                        \
    regs.r10 = rm["r10"];                                                      \
    regs.r11 = rm["r11"];                                                      \
    regs.r12 = rm["r12"];                                                      \
    regs.r13 = rm["r13"];                                                      \
    regs.r14 = rm["r14"];                                                      \
    regs.r15 = rm["r15"];                                                      \
    regs.rbx = rm["rbx"];                                                      \
    regs.rbp = rm["rbp"];                                                      \
    regs.rsp = rm["rsp"];                                                      \
    regs.eflags = rm["efl"];                                                   \
    regs.rip = rm["rip"]

#define run_requirement(is_running, msg)                                       \
    if (!is_running) {                                                         \
        std::cout << msg << std::endl;                                         \
        continue;                                                              \
    }

/**
 * @brief Represents a breakpoint in the debugger.
 *
 * A breakpoint is a specific location in the code where the debugger will pause
 * the execution of the program for inspection or debugging purposes.
 */
struct breakpoint {
    /**
     * @brief The memory address.
     */
    unsigned long addr;
    /**
     * @brief The original data value.
     */
    long original_data;
};

/**
 * @brief Array of breakpoints.
 *
 * This static array stores the breakpoints set by the debugger. It has a
 * maximum size of MAX_BREAKPOINTS.
 */
static struct breakpoint breakpoints[MAX_BREAKPOINTS];
/**
 * @brief The number of breakpoints set in the debugger.
 */
static int breakpoint_count;

/**
 * @class Debugger
 * @brief Represents a debugger for a target process.
 *
 * The Debugger class provides functionality to debug a target process,
 * including starting and killing the process, retrieving information about
 * local variables, stepping through the code, setting breakpoints, and more.
 */
class Debugger {
    /**
     * @brief Represents a process ID.
     *
     * The `pid_t` type is used to represent a process ID. It is a signed
     * integer type that is capable of representing any valid process ID on the
     * system.
     */
    pid_t c_pid;
    /**
     * @brief Indicates whether the debugger is started or not.
     */
    bool is_started;
    /**
     * @brief The target being debugged.
     */
    const char *target;
    /**
     * @brief Pointer to the DwarfInfo object.
     */
    DwarfInfo *DwInfo;
    /**
     * @brief Pointer to a Disassm object.
     */
    Disassm *disaska;

  private:
    /**
     * @brief Spawns a target for debugging.
     */
    void spawn_target();
    /**
     * @brief Runs the debugger.
     */
    void run_debugger();

  public:
    /**
     * @brief Constructs a Debugger object with the given configuration.
     *
     * @param cfg The configuration for the debugger.
     */
    Debugger(Configuration cfg);

    /**
     * @brief Starts the target process with the given process ID.
     *
     * @param gp A pointer to the process ID of the target process.
     */
    void start(pid_t *gp);

    /**
     * @brief Kills the target process.
     */
    void kill_target();

    /**
     * @brief Retrieves information about local variables.
     */
    void info_locals();

    // Debugger commands

    /**
     * @brief Executes the next instruction and stops.
     *
     * @param status A pointer to the status of the execution.
     */
    void step(int *status);

    /**
     * @brief Executes an unknown command.
     */
    void unknown();

    /**
     * @brief Retrieves information about the registers.
     */
    void info_regs();

    /**
     * @brief Lists all the functions in the target process.
     */
    void list_functions();

    /**
     * @brief Sets a breakpoint at the specified address.
     *
     * @param addr The address where the breakpoint should be set.
     */
    void set_breakpoint(uint64_t addr);

    /**
     * @brief Continues the execution of the target process.
     *
     * @param status A pointer to the status of the execution.
     */
    void continue_execution(int *status);

    /**
     * @brief Disassembles the current instruction.
     */
    void disassemble();

    /**
     * @brief Reads memory at the specified address.
     */
    void x_read();

    /**
     * @brief Sets memory at the specified address.
     */
    void x_set();

    /**
     * @brief Executes the next instruction and stops.
     *
     * @param status A pointer to the status of the execution.
     */
    void next(int *status);

    /**
     * @brief Prints the value of the current instruction.
     */
    void print();
};

#endif