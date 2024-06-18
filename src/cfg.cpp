#include "cfg.hpp"
#include "utils.hpp"

const char *Configuration::get_path() { return path; }

bool Configuration::validate() { return access(path, F_OK) != -1; }

Configuration::Configuration(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s PATH\n", argv[0]);
        panic("incorrect parameters");
    }

    path = argv[1];
    if (!validate()) {
        panic("bad target (not found)");
    }
}