#ifndef DWARF_INFO
#define DWARF_INFO

#include <cstdint>
#include <dwarf.h>
#include <fcntl.h>
#include <iostream>
#include <libdwarf.h>
#include <map>

/**
 * @brief The DwarfInfo class provides functionality to retrieve information
 * from DWARF debugging information.
 *
 * The class encapsulates a DWARF_Debug object and provides methods to
 * initialize the DWARF_Debug object, retrieve function information by RIP
 * (Instruction Pointer), retrieve local variables of a function, and traverse
 * the DWARF tree.
 */
class DwarfInfo {
    /**
     * @brief Represents a handle to a DWARF debugging session.
     *
     * The `Dwarf_Debug` structure is used to manage a DWARF debugging session.
     * It provides functions and data structures for accessing and manipulating
     * DWARF debugging information.
     */
    Dwarf_Debug dbg;
    /**
     * @brief Represents an error in the DWARF library.
     *
     * This structure is used to store information about an error that occurred
     * during DWARF library operations. It provides details such as the error
     * code and a human-readable error message.
     */
    Dwarf_Error err;

  public:
    /**
     * @brief Initializes the DWARF information.
     *
     * This function initializes the DWARF information, which is used for
     * debugging purposes. It should be called before any other DWARF-related
     * functions are used.
     */
    void dw_init();
    /**
     * @brief Retrieves the function information associated with the given
     * instruction pointer (rip).
     *
     * @param rip The instruction pointer value.
     * @param ret_string A reference to a string that will be populated with the
     * function name.
     * @param low_pc A reference to a variable that will be populated with the
     * low_pc value of the function.
     * @param high_pc A reference to a variable that will be populated with the
     * high_pc value of the function.
     */
    void get_function_by_rip(Dwarf_Addr rip, std::string &ret_string,
                             Dwarf_Addr &low_pc, Dwarf_Addr &high_pc);
    /**
     * @brief Retrieves the local variables of a given function.
     *
     * @param func_name The name of the function.
     * @return A map containing the names and values of the local variables.
     */
    std::map<std::string, uint64_t> get_local_vars(char *func_name);
    /**
     * @brief Constructs a `DwarfInfo` object.
     *
     * This constructor initializes a `DwarfInfo` object with the provided
     * target and child process ID.
     *
     * @param target_ The target name or path.
     * @param child_pid_ The process ID of the child process.
     */
    DwarfInfo(const char *target_, pid_t child_pid_)
        : target{target_}, child_pid{child_pid_} {}

  private:
    /**
     * @brief Traverses the DWARF tree starting from the given DIE and collects
     * information about functions.
     *
     * @param dbg The Dwarf_Debug object representing the DWARF debugging
     * information.
     * @param die The Dwarf_Die object representing the starting DIE in the
     * DWARF tree.
     * @param func_name The name of the function being traversed.
     * @param res A reference to a std::map object that will store the collected
     * information.
     */
    void traverse_dwarf_tree(Dwarf_Debug dbg, Dwarf_Die die, char *func_name,
                             std::map<std::string, uint64_t> &res);
    /**
     * @brief Retrieves the offset of the entry at the specified index in the
     * given Dwarf_Loc_Head_c structure.
     *
     * @param dw_loclist_head The Dwarf_Loc_Head_c structure containing the
     * debug information entries.
     * @param i The index of the entry to retrieve the offset for.
     * @param offset A reference to a Dwarf_Off variable where the offset will
     * be stored.
     * @return An integer indicating the success or failure of the operation.
     */
    int dwarf_get_entry_offset(Dwarf_Loc_Head_c dw_loclist_head,
                               Dwarf_Unsigned i, Dwarf_Off &offset);
    /**
     * @brief The target being debugged.
     */
    const char *target;
    /**
     * @brief The process ID of the child process.
     */
    pid_t child_pid;
};

#endif