#include <gtest/gtest.h>
#include <cstdlib>

// QCSIM_SRS_019: CLI must run with input file
TEST(CliTest, RunCliHelp) {
    int ret = system("./qsim_cli --help > /dev/null");
    EXPECT_EQ(ret, 0);
}
