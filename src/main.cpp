#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>

#include "utils.hpp"

class Configuration {
  public:
    Configuration(int argc, char **argv);
};

class Debugger {
  public:
    Debugger(Configuration cfg);
    void start(pid_t* gp);
    void kill_target();
};

pid_t global_pid = -1;

void dbg_sgh(int sig) {
    if (sig == SIGINT && global_pid != 0) {
        std::cout << std::endl
                  << "Debugger interrupted, stopping child process"
                  << std::endl;
        ptrace(PTRACE_INTERRUPT, global_pid, 0, 0);
    }
}

int main(int argc, char *argv[]) {
    Configuration cfg(argc, argv);

    signal(SIGINT, dbg_sgh);
    o_log("registered the signal handler", "SIGINT");

    Debugger dbg(cfg);
    dbg.start(&global_pid);

    dbg.kill_target();
    return 0;
}