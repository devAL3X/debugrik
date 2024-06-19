#ifndef DWARF_INFO
#define DWARF_INFO

#include <dwarf.h>
#include <fcntl.h>
#include <iostream>
#include <libdwarf.h>

class DwarfInfo {
    Dwarf_Debug dbg;
    Dwarf_Error err;

  public:
    void dw_init();
    void get_function_name_by_rip(Dwarf_Addr rip, std::string &ret_string);
    void print_local_vars();
    DwarfInfo(const char *target_, pid_t child_pid_)
        : target{target_}, child_pid{child_pid_} {
        dw_init();
    }

  private:
    void traverse_dwarf_tree(Dwarf_Debug dbg, Dwarf_Die die);
    int dwarf_get_entry_offset(Dwarf_Loc_Head_c dw_loclist_head,
                               Dwarf_Unsigned i, Dwarf_Off &offset);
    const char *target;
    pid_t child_pid;
};

#endif