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
    void get_function_name_by_rip(Dwarf_Addr rip);
    void print_local_vars_and_values(Dwarf_Debug dbg, Dwarf_Die die);
    void read_dwarf_info();
    int dwarf_get_entry_offset(Dwarf_Loc_Head_c dw_loclist_head,
                               Dwarf_Unsigned i, Dwarf_Off &offset);
    DwarfInfo(const char *target_, pid_t child_pid_)
        : target{target_}, child_pid{child_pid_} {
        if (dwarf_init_path(target, nullptr, 0, DW_GROUPNUMBER_ANY, nullptr,
                            nullptr, &dbg, &err) != DW_DLV_OK) {
            std::cerr << "dwarf_init_path() failed." << std::endl;
            return;
        }
    }

  private:
    const char *target;
    pid_t child_pid;
};

#endif