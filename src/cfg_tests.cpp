#include "cfg.hpp"

#include <cstring>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>

bool panic_triggered = false;

void panic(char *arg) {
    // ... mock panic
    panic_triggered = true;
}

#define TEST_PATH_CORRECT "/etc/hosts"
#define TEST_PATH_INCORRECT "/k/a/c"

TEST(ConfigTestSuite, cfg_load_correct_file) {
    int argc = 2;
    char *argv[argc];

    argv[0] = strdup("testik");
    argv[1] = strdup(TEST_PATH_CORRECT);

    // Configuration cfg(TEST_PATH_CORRECT);

    Configuration cfg(argc, argv);
    ASSERT_TRUE(strcmp(cfg.get_path(), TEST_PATH_CORRECT) == 0);
}

TEST(ConfigTestSuite, cfg_bad_file_specified) {
    int argc = 2;
    char *argv[argc];

    argv[0] = strdup("testik");
    argv[1] = strdup(TEST_PATH_INCORRECT);

    Configuration cfg(argc, argv);
    ASSERT_TRUE(strcmp(cfg.get_path(), TEST_PATH_CORRECT) != 0);
    ASSERT_TRUE(panic_triggered);
}