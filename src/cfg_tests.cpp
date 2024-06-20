#include "cfg.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstring>


class MockPanic {
public:
    MOCK_METHOD(void, panic, (char *cmd));
};
MockPanic *mock_panic = nullptr;


#define TEST_PATH_CORRECT   "/etc/hosts"
#define TEST_PATH_INCORRECT "/k/a/c"

TEST(ConfigTestSuite, get_path_correct) {
    int argc = 2;
    char *argv[argc];
    
    argv[0] = strdup("testik");
    argv[1] = strdup("testik");

    // Configuration cfg(TEST_PATH_CORRECT);

    Configuration cfg(argc, argv);

    ASSERT_TRUE(true);
}