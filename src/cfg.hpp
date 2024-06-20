#ifndef CFG_H
#define CFG_H

#include <unistd.h>

/**
 * @brief The Configuration class represents the configuration settings for the
 * application.
 *
 * This class provides methods to validate and retrieve the configuration path.
 */
class Configuration {
    const char *path;

  private:
    /**
     * @brief Validates the configuration.
     *
     * This function checks if the configuration is valid and returns a boolean
     * value indicating the result.
     *
     * @return true if the configuration is valid, false otherwise.
     */
    bool validate();

  public:
    /**
     * @brief Constructs a new Configuration object with the specified command
     * line arguments.
     *
     * @param argc The number of command line arguments.
     * @param argv An array of command line argument strings.
     */
    Configuration(int argc, char **argv);

    /**
     * @brief Gets the configuration path.
     *
     * @return The configuration path as a null-terminated string.
     */
    const char *get_path();
};

#endif