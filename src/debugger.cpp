#include "disassm.hpp"
#include "dwarfinfo.hpp"
#include "arch.hpp"
#include "debugger.hpp"
#include "utils.hpp"

#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <string.h>
#include <string>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utility>
#include <vector>

#include <dwarf.h>
#include <libdwarf.h>

/* Global TODO:
- get rid of `prinf`
- move all registers operations to separate function
*/

Debugger::Debugger(Configuration cfg) : is_started(false) {
    target = cfg.get_path();
    disaska = new Disassm;
}

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
        DwInfo = new DwarfInfo(target, c_pid);
        run_debugger();
    } else {
        panic("failed to fork (startup error)");
    }
}

void Debugger::run_debugger() {
    int wait_status;
    wait(&wait_status);

    outer: while (WIFSTOPPED(wait_status)) {
        std::string inp;

        std::cout << "dbg> ";
        std::cin >> inp;

        if (inp == "c") {
            continue_execution(&wait_status);
        } else if (inp == "b") {
            uint64_t addr;
            std::cin >> std::hex >> addr;

            set_breakpoint(addr);
            std::cout <<  "Breakpoint set at: " << std::hex << (void *) addr << std::endl;
        } else if (inp == "exit") {
            std::cout << "bye" << std::endl;
            break;
        } else if (inp == "ir") {
            run_requirement(is_started, MSG_SHOULD_BE_RUNNED) info_regs();
        } else if (inp == "s") {
            run_requirement(is_started, MSG_SHOULD_BE_RUNNED) step(&wait_status);
        } else if (inp == "il") {
            run_requirement(is_started, MSG_SHOULD_BE_RUNNED) info_locals();
        } else if (inp == "lf") {
            list_functions();
        } else if (inp == "dis") {
            run_requirement(is_started, MSG_SHOULD_BE_RUNNED) disassemble();
        } else if (inp == "r") {
            run_requirement(!is_started, MSG_ALREADY_STARTED) continue_execution(&wait_status);
        } else if (inp == "x") {
            run_requirement(is_started, MSG_SHOULD_BE_RUNNED) x_read();
        } else if(inp == "set") {
            run_requirement(is_started, MSG_SHOULD_BE_RUNNED) x_set();
        } else if(inp == "n") {
            run_requirement(is_started, MSG_SHOULD_BE_RUNNED) next(&wait_status);
        } else {
            unknown();
        }
    }
}

void Debugger::next(int *status) {
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, c_pid, 0, &regs);

    uint8_t buf[MAX_INSTR_SIZE+1];
    read_process_memory(c_pid, regs.rip, buf, MAX_INSTR_SIZE);

    uint64_t *curr_instr_sz = disaska->next_instr_addr(buf, MAX_INSTR_SIZE, regs.rip);
    // std::cout << curr_instr_sz << std::endl;

    // set_breakpoint((uint64_t) curr_instr_sz);
    

    if(curr_instr_sz != nullptr) {
        set_breakpoint((uint64_t) curr_instr_sz);
        continue_execution(status);
    } else {
        step(status);
    }


    // std::cout << disaska->handle << std::endl;
    // Get next after CUR_INSTR instruction address
}

void Debugger::x_set() {
    uint64_t val;
    std::string reg;
    
    std::cin >> reg;
    std::cin >> std::hex >> val;

    // Get actual register values
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, c_pid, 0, &regs);
    std::map<std::string, unsigned long long> rm = expand_regs(regs);

    std::cout << reg << std::endl;
    if(rm.count(reg) == 0) {
        std::cout << "bad register name" << std::endl;
        return;
    }

    rm[reg] = val;
    flatten_regs(regs, rm);
    ptrace(PTRACE_SETREGS, c_pid, 0, &regs);
}

void Debugger::x_read() {
    unsigned long addr, k;
    
    std::cin >> std::hex >> addr;
    std::cin >> k;
    
    if(k > MAX_XREAD_K) {
        std::cout << "to much bytes to read!" << std::endl;
        return;
    }
    uint64_t memo[k]; 

    read_process_memory(c_pid, addr, (uint8_t *) memo, sizeof(uint64_t) * k);
    dump((void *)addr, memo, k);
}

void Debugger::continue_execution(int *wait_status) {
    ptrace(PTRACE_CONT, c_pid, 0, 0);
    is_started = true;
    
    wait(wait_status);

    // Then we got to breakpoint
    if (WIFSTOPPED(*wait_status) && WSTOPSIG(*wait_status) == SIGTRAP) {
        struct user_regs_struct regs;
        ptrace(PTRACE_GETREGS, c_pid, 0, &regs);

        for (int i = 0; i < breakpoint_count; i++) {
            if (regs.rip - 1 == breakpoints[i].addr) {
                // If it's our breakpoint
                printf("Breakpoint hit at 0x%lx\n", breakpoints[i].addr);

                // Restore original instruction
                ptrace(PTRACE_POKETEXT, c_pid, (void *)breakpoints[i].addr,
                       (void *)breakpoints[i].original_data);

                // Execute instructions after restoring
                regs.rip -= 1;
                ptrace(PTRACE_SETREGS, c_pid, 0, &regs);
                ptrace(PTRACE_SINGLESTEP, c_pid, 0, 0);

                // Wait until next breakpoint?
                wait(wait_status);

                // Reinsert prev breakpoint
                ptrace(PTRACE_POKETEXT, c_pid, (void *)breakpoints[i].addr,
                       (void *)(breakpoints[i].original_data & LSB_TRAP_MASK |
                                TRAP_BYTE));
                break;
            }
        }
    }
}

void Debugger::step(int *wait_status) {
    ptrace(PTRACE_SINGLESTEP, c_pid, 0, 0);
    wait(wait_status);
}

void Debugger::info_regs() {
    std::string name;
    struct user_regs_struct regs;

    ptrace(PTRACE_GETREGS, c_pid, 0, &regs);


    std::map<std::string, unsigned long long> p_map = expand_regs(regs);

    std::cout << "Registers:" << std::endl;

    int modulus = 0;
    for (auto kv: p_map) {
        std::cout << kv.first << "=" << std::setw(16) << std::setfill('0')
                  << std::hex << kv.second << " ";

        if (++modulus % 3 == 0) {
            std::cout << std::endl;
        }
    }

    // last newline not sent yet
    if (modulus % 3 != 0)
        std::cout << std::endl;
}

void Debugger::disassemble() {
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, c_pid, 0, &regs);

    Dwarf_Addr low_pc, high_pc, tmp_low_pc, tmp_high_pc;
    std::string name;

    DwInfo->get_function_by_rip(regs.rip, name, low_pc, high_pc);
    uint8_t *code = new uint8_t[high_pc - low_pc];

    if (!read_process_memory(c_pid, low_pc, code, high_pc - low_pc)) {
        perror("ptrace read");
        ptrace(PTRACE_DETACH, c_pid, nullptr, nullptr);
    } else {
        // Loop over all breakpoints for reverting changes: now dump contains
        // TRAP instructions
        for (int i = 0; i < breakpoint_count; i++) {
            std::string breakpoint_position;
            DwInfo->get_function_by_rip(breakpoints[i].addr,
                                        breakpoint_position, tmp_low_pc,
                                        tmp_high_pc);

            if (name == breakpoint_position) {
                // patch_idx contains trap byte
                int patch_idx = breakpoints[i].addr - tmp_low_pc;
                code[patch_idx] = (uint8_t)breakpoints[i].original_data;
            }
        }

        std::cout << "assembly:" << std::endl;
        disaska->print_disassembly(code, high_pc - low_pc, low_pc);
    }

    delete[] code;
}

void Debugger::list_functions() {
    std::string cmd = "nm " + std::string(target) + " | grep '.* T .*'";
    system(cmd.c_str());
}

void Debugger::info_locals() {
    struct user_regs_struct regs;
    if (ptrace(PTRACE_GETREGS, c_pid, 0, &regs) < 0) {
        perror("ptrace(GETREGS)");
        exit(EXIT_FAILURE);
    }

    Dwarf_Addr low_pc, high_pc;
    std::string func_name;
    DwInfo->get_function_by_rip(regs.rip, func_name, low_pc, high_pc);
    DwInfo->print_local_vars((char *)func_name.c_str());
}

void Debugger::set_breakpoint(uint64_t addr) {
    std::cout << "Setting the breakpoint to: " << std::hex << (void *) addr << std::endl; 
    long data = ptrace(PTRACE_PEEKTEXT, c_pid, (void *)addr, 0);
    long breakpoint = (data & LSB_TRAP_MASK) | TRAP_BYTE;

    // Track using breakpoints
    breakpoints[breakpoint_count].addr = addr;
    breakpoints[breakpoint_count].original_data = data;
    breakpoint_count++;

    // Change the real instruction
    ptrace(PTRACE_POKETEXT, c_pid, (void *)addr, (void *)breakpoint);
}


void Debugger::unknown() {
    std::cout << "unknown command" << std::endl;
}