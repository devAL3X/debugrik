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

#define MSG_SHOULD_BE_RUNNED "target not started"
#define MSG_ALREADY_STARTED  "target is already in run"


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

};



#endif