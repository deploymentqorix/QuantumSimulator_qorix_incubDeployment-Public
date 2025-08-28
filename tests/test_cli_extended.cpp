#include <gtest/gtest.h>
#include <cstdlib>

// CLI with invalid input
TEST(CliExtended, InvalidInput) {
    int ret = system("./qsim_cli invalid_file.qasm > /dev/null 2>&1");
    EXPECT_NE(ret, 0);
}
