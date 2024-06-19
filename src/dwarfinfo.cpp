#include "dwarfinfo.hpp"

#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utility>
#include <vector>

#include <dwarf.h>
#include <libdwarf.h>
#include <string.h>

void DwarfInfo::dw_init() {
    if (dwarf_init_path(target, nullptr, 0, DW_GROUPNUMBER_ANY, nullptr,
                        nullptr, &dbg, &err) != DW_DLV_OK) {
        std::cerr << "dwarf_init_path() failed." << std::endl;
        return;
    }
};

void DwarfInfo::get_function_by_rip(Dwarf_Addr rip, std::string &ret_string,
                                    Dwarf_Addr &low_pc, Dwarf_Addr &high_pc) {
    dw_init();
    Dwarf_Unsigned cu_header_length, abbrev_offset, next_cu_header,
        dw_typeoffset, dw_next_cu_header_offset;
    Dwarf_Half dw_version_stamp, dw_address_size, dw_length_size,
        dw_extension_size, dw_header_cu_type;
    Dwarf_Unsigned dw_cu_header_length;
    Dwarf_Off dw_abbrev_offset;
    Dwarf_Sig8 dw_type_signature;
    Dwarf_Die no_die = 0, cu_die, child_die;

    while (dwarf_next_cu_header_d(dbg, true, &dw_cu_header_length,
                                  &dw_version_stamp, &dw_abbrev_offset,
                                  &dw_address_size, &dw_length_size,
                                  &dw_extension_size, &dw_type_signature,
                                  &dw_typeoffset, &dw_next_cu_header_offset,
                                  &dw_header_cu_type, &err) == DW_DLV_OK) {

        if (dwarf_siblingof_b(dbg, no_die, true, &cu_die, &err) == DW_DLV_OK) {
            if (dwarf_child(cu_die, &child_die, &err) == DW_DLV_OK) {
                do {
                    Dwarf_Half tag;
                    if (dwarf_tag(child_die, &tag, &err) != DW_DLV_OK) {
                        continue;
                    }
                    if (tag == DW_TAG_subprogram) {
                        dwarf_lowpc(child_die, &low_pc, &err);
                        Dwarf_Half dw_return_form;
                        enum Dwarf_Form_Class dw_return_class;
                        dwarf_highpc_b(child_die, &high_pc, &dw_return_form,
                                       &dw_return_class, &err);
                        high_pc += low_pc;
                        if (rip >= low_pc && rip < high_pc) {
                            char *name = 0;
                            if (dwarf_diename(child_die, &name, &err) ==
                                DW_DLV_OK) {
                                ret_string = std::string(name);
                                return;
                            }
                        }
                    }
                } while (dwarf_siblingof_b(dbg, child_die, true, &child_die,
                                           &err) == DW_DLV_OK);
            }
        }
    }
}

int DwarfInfo::dwarf_get_entry_offset(Dwarf_Loc_Head_c dw_loclist_head,
                                      Dwarf_Unsigned i, Dwarf_Off &offset) {
    Dwarf_Small dw_lle_value_out;
    Dwarf_Unsigned dw_rawlowpc;
    Dwarf_Unsigned dw_rawhipc;
    Dwarf_Bool dw_debug_addr_unavailable;
    Dwarf_Addr dw_lowpc_cooked;
    Dwarf_Addr dw_hipc_cooked;
    Dwarf_Unsigned dw_locexpr_op_count_out;
    Dwarf_Locdesc_c dw_locentry_out;
    Dwarf_Small dw_loclist_source_out;
    Dwarf_Unsigned dw_expression_offset_out;
    Dwarf_Unsigned dw_locdesc_offset_out;

    // get i-th element of location list
    if (dwarf_get_locdesc_entry_d(
            dw_loclist_head, i, &dw_lle_value_out, &dw_rawlowpc, &dw_rawhipc,
            &dw_debug_addr_unavailable, &dw_lowpc_cooked, &dw_hipc_cooked,
            &dw_locexpr_op_count_out, &dw_locentry_out, &dw_loclist_source_out,
            &dw_expression_offset_out, &dw_locdesc_offset_out,
            &err) == DW_DLV_OK) {

        Dwarf_Small dw_operator_out;
        Dwarf_Unsigned dw_operand1;
        Dwarf_Unsigned dw_operand2;
        Dwarf_Unsigned dw_operand3;
        Dwarf_Unsigned dw_offset_for_branch;

        // get operator's first arg.
        // in case of location it means that we take DW_OP_fbreg arg
        // wich actually is an rbp offset - 0x10
        if (dwarf_get_location_op_value_c(dw_locentry_out, 0, &dw_operator_out,
                                          &dw_operand1, &dw_operand2,
                                          &dw_operand3, &dw_offset_for_branch,
                                          &err) == DW_DLV_OK) {

            offset = dw_operand1;
            return DW_DLV_OK;
        }
        return DW_DLV_ERROR;
    }
    return DW_DLV_ERROR;
}

void DwarfInfo::traverse_dwarf_tree(Dwarf_Debug dbg, Dwarf_Die die,
                                    char *func_name) {

    Dwarf_Half tag;
    if (dwarf_tag(die, &tag, &err) != DW_DLV_OK) {
        return;
    }

    // we are interested only in variable entries
    if (tag == DW_TAG_variable) {
        // get entty name
        char *die_name = 0;
        if (dwarf_diename(die, &die_name, &err) != DW_DLV_OK) {
            return;
        }
        // Get the location of the variable
        Dwarf_Attribute attr;
        Dwarf_Loc_Head_c dw_loclist_head;
        Dwarf_Unsigned dw_locentry_count;

        Dwarf_Off offset;

        if (dwarf_attr(die, DW_AT_location, &attr, &err) == DW_DLV_OK) {
            if (dwarf_get_loclist_c(attr, &dw_loclist_head, &dw_locentry_count,
                                    &err) == DW_DLV_OK) {
                for (Dwarf_Unsigned i = 0; i < dw_locentry_count; i++) {

                    if (dwarf_get_entry_offset(dw_loclist_head, i, offset) !=
                        DW_DLV_OK) {
                        return;
                    }
                }
                dwarf_dealloc_loc_head_c(dw_loclist_head);
            }

            struct user_regs_struct regs;
            if (ptrace(PTRACE_GETREGS, child_pid, 0, &regs) < 0) {
                perror("ptrace(GETREGS)");
                exit(EXIT_FAILURE);
            }

            unsigned long addr = regs.rbp + 0x10 + offset;
            long value =
                ptrace(PTRACE_PEEKDATA, child_pid, (void *)addr, nullptr);

            printf("Variable: %s, Value: %llx\n", die_name, value);
        }
    }

    Dwarf_Die child;
    if (dwarf_child(die, &child, &err) == DW_DLV_OK) {
        Dwarf_Half tag;
        char *name = 0;
        dwarf_tag(die, &tag, &err);
        dwarf_diename(die, &name, &err);
        if (!(tag == DW_TAG_subprogram && strcmp(name, func_name) != 0)) {
            traverse_dwarf_tree(dbg, child, func_name);
        }
    }

    Dwarf_Die sibling;
    if (dwarf_siblingof_b(dbg, die, true, &sibling, &err) == DW_DLV_OK) {
        traverse_dwarf_tree(dbg, sibling, func_name);
    }
}

// Function to read DWARF info and extract local variables
void DwarfInfo::print_local_vars(char *func_name) {
    dw_init();
    Dwarf_Unsigned cu_header_length, abbrev_offset, next_cu_header,
        dw_typeoffset, dw_next_cu_header_offset;
    Dwarf_Half dw_version_stamp, dw_address_size, dw_length_size,
        dw_extension_size, dw_header_cu_type;
    Dwarf_Unsigned dw_cu_header_length;
    Dwarf_Off dw_abbrev_offset;
    Dwarf_Sig8 dw_type_signature;

    while (dwarf_next_cu_header_d(dbg, true, &dw_cu_header_length,
                                  &dw_version_stamp, &dw_abbrev_offset,
                                  &dw_address_size, &dw_length_size,
                                  &dw_extension_size, &dw_type_signature,
                                  &dw_typeoffset, &dw_next_cu_header_offset,
                                  &dw_header_cu_type, &err) == DW_DLV_OK) {
        Dwarf_Die no_die, cu_die;

        if (dwarf_siblingof_b(dbg, no_die, true, &cu_die, &err) == DW_DLV_OK) {
            traverse_dwarf_tree(dbg, cu_die, func_name);
        }
    }
}
