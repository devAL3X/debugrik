#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "cfg.hpp"
#include "utils.hpp"

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/reg.h>

#define MAX_BREAKPOINTS 100

struct breakpoint {
    unsigned long addr;
    long original_data;
};

static struct breakpoint breakpoints[MAX_BREAKPOINTS];
static int breakpoint_count;

class Debugger {
    pid_t c_pid;
    const char *target;

private:
    void spawn_target();
    void run_debugger();

public:
    Debugger(Configuration cfg);
    
    void start(pid_t *gp);
    void kill_target();
};

#endif