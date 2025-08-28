#include <gtest/gtest.h>
#include <cstdlib>

// Server basic run (just starts and exits help)
TEST(ServerExtended, HelpOption) {
    int ret = system("./qsim_server --help > /dev/null 2>&1");
    EXPECT_EQ(ret, 0);
}
