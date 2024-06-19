#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "cfg.hpp"
#include "utils.hpp"
#include "dwarfinfo.hpp"
#include "disassm.hpp"

#include <map>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>

#define MAX_BREAKPOINTS 100
#define MAX_XREAD_K     8

#define MSG_SHOULD_BE_RUNNED "target not started"
#define MSG_ALREADY_STARTED  "target is already in run"


#define expand_regs(regs) { \
    {"rdi", regs.rdi}, {"rsi", regs.rsi}, {"rdx", regs.rdx}, \
    {"rcx", regs.rcx}, {"rax", regs.rax}, {" r8", regs.r8}, \
    {" r9", regs.r9},  {"r10", regs.r10}, {"r11", regs.r11}, \
    {"r12", regs.r12}, {"r13", regs.r13}, {"r14", regs.r14}, \
    {"r15", regs.r15}, {"rbx", regs.rbx}, {"rbp", regs.rbp}, \
    {"rsp", regs.rsp}, {"rip", regs.rip}, {"efl", regs.eflags} \
};

#define flatten_regs(regs, rm) \
  regs.rdi = rm["rdi"]; \
  regs.rsi = rm["rsi"]; \
  regs.rdx = rm["rdx"]; \
  regs.rcx = rm["rcx"]; \
  regs.rax = rm["rax"]; \
  regs.r8 = rm["r8"]; \
  regs.r9 = rm["r9"]; \
  regs.r10 = rm["r10"]; \
  regs.r11 = rm["r11"]; \
  regs.r12 = rm["r12"]; \
  regs.r13 = rm["r13"]; \
  regs.r14 = rm["r14"]; \
  regs.r15 = rm["r15"]; \
  regs.rbx = rm["rbx"]; \
  regs.rbp = rm["rbp"]; \
  regs.rsp = rm["rsp"]; \
  regs.eflags = rm["efl"]; \
  regs.rip = rm["rip"]

#define run_requirement(is_running, msg) \
    if(!is_running) { \
        std::cout << msg << std::endl; \
        continue; \
    } \

struct breakpoint {
    unsigned long addr;
    long original_data;
};

static struct breakpoint breakpoints[MAX_BREAKPOINTS];
static int breakpoint_count;

class Debugger {
    pid_t c_pid;
    bool is_started;
    const char *target;
    DwarfInfo *DwInfo;
    Disassm *disaska;

  private:
    void spawn_target();
    void run_debugger();

  public:
    Debugger(Configuration cfg);

    void start(pid_t *gp);
    void kill_target();
    void info_locals();

    // Debugger commands
    void step(int *status);
    void unknown();
    void info_regs();
    void list_functions();
    void set_breakpoint();
    void continue_execution(int *status);
    void disassemble();
    void x_read();
    void x_set();

};



#endif