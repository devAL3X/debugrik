#include "debugger.hpp"
#include "dwarfinfo.hpp"
#include "utils.hpp"

#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utility>
#include <vector>

#include <dwarf.h>
#include <libdwarf.h>

#include "dwarfinfo.hpp"
/* Global TODO:
- get rid of `target`
- get rid of `prinf`
- add ^C and ^D handling
*/


void info_locals(pid_t child_pid, const char *target) {
    auto dwInfo = DwarfInfo(target, child_pid);
    struct user_regs_struct regs;
    if (ptrace(PTRACE_GETREGS, child_pid, 0, &regs) < 0) {
        perror("ptrace(GETREGS)");
        exit(EXIT_FAILURE);
    }

    printf("Breakpoint hit at address: %llx\n", regs.rip);

    dwInfo.read_dwarf_info();
}

Debugger::Debugger(Configuration cfg) { target = cfg.get_path(); }

void Debugger::spawn_target() {
    o_log("spawning the target", target);
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) == -1) {
        panic("ptrace failed");
    }
    execl(target, target, nullptr);
}

void Debugger::kill_target() { ptrace(PTRACE_KILL, c_pid); }

void Debugger::start(pid_t *gp) {
    c_pid = fork();
    *gp = c_pid;

    if (c_pid == 0) {
        spawn_target();
    } else if (c_pid > 0) {
        run_debugger();
    } else {
        panic("failed to fork (startup error)");
    }
}

void Debugger::run_debugger() {
    int wait_status;
    struct user_regs_struct regs;

    wait(&wait_status);
    while (WIFSTOPPED(wait_status)) {
        std::string inp;

        std::cout << "dbg> ";
        std::cin >> inp;

        if (inp == "c") {
            ptrace(PTRACE_CONT, c_pid, 0, 0);
            wait(&wait_status);

            // Then we got to breakpoint
            if (WIFSTOPPED(wait_status) && WSTOPSIG(wait_status) == SIGTRAP) {
                ptrace(PTRACE_GETREGS, c_pid, 0, &regs);

                for (int i = 0; i < breakpoint_count; i++) {
                    if (regs.rip - 1 == breakpoints[i].addr) {
                        printf("Breakpoint hit at 0x%lx\n",
                               breakpoints[i].addr);

                        // Restore original instruction
                        ptrace(PTRACE_POKETEXT, c_pid,
                               (void *)breakpoints[i].addr,
                               (void *)breakpoints[i].original_data);

                        // Move RIP back to the breakpoint address
                        regs.rip -= 1;
                        ptrace(PTRACE_SETREGS, c_pid, 0, &regs);

                        // Single step to execute original instruction
                        ptrace(PTRACE_SINGLESTEP, c_pid, 0, 0);

                        wait(&wait_status);

                        // Reinsert breakpoint
                        ptrace(PTRACE_POKETEXT, c_pid,
                               (void *)breakpoints[i].addr,
                               (void *)(breakpoints[i].original_data &
                                            0xffffffffffffff00 |
                                        0xcc));
                        break;
                    }
                }
            }
        } else if (inp == "b") {
            unsigned long addr;

            std::cout << "addr: ";
            std::cin >> std::hex >> addr;

            long data = ptrace(PTRACE_PEEKTEXT, c_pid, (void *)addr, 0);

            long breakpoint = (data & 0xffffffffffffff00) | 0xcc;

            // Save original data
            breakpoints[breakpoint_count].addr = addr;
            breakpoints[breakpoint_count].original_data = data;
            breakpoint_count++;

            ptrace(PTRACE_POKETEXT, c_pid, (void *)addr, (void *)breakpoint);
            printf("Breakpoint set at 0x%lx\n", addr);
        } else if (inp == "exit") {
            std::cout << "bye" << std::endl;
            break;
        } else if (inp == "ir") {
            ptrace(PTRACE_GETREGS, c_pid, 0, &regs);
            printf("RIP: 0x%llx\n", regs.rip);
            auto dwInfo = DwarfInfo(target, c_pid);
            dwInfo.get_function_name_by_rip(regs.rip);
            // get_function_name_by_rip(regs.rip, target);
        } else if (inp == "s") {
            ptrace(PTRACE_SINGLESTEP, c_pid, 0, 0);
            wait(&wait_status);
        } else if (inp == "il") {
            std::cout << "Locals" << std::endl;

            info_locals(c_pid, target);

        } else {
            std::cout << "unknown command" << std::endl;
        }
    }
}