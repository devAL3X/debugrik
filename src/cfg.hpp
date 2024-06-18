#ifndef CFG_H
#define CFG_H

#include <unistd.h>


class Configuration {
    const char *path;

private:
    bool validate();

public:
    Configuration(int argc, char **argv);

    const char *get_path();
};

#endif