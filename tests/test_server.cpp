#include <gtest/gtest.h>
#include <cstdlib>

// QCSIM_SRS_020: Server shall run and accept requests
TEST(ServerTest, RunServer) {
    int ret = system("./qsim_server --help > /dev/null");
    EXPECT_EQ(ret, 0);
}
