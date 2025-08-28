#include <gtest/gtest.h>
#include "DepolarizingChannel.h"
#include "PhaseFlipChannel.h"

// QCSIM_SRS_016: Depolarizing channel
TEST(NoiseTest, DepolarizingChannel) {
    DepolarizingChannel dc(0.1);
    auto result = dc.apply(0);
    EXPECT_GE(result.size(), 1);
}

// QCSIM_SRS_018: PhaseFlip channel
TEST(NoiseTest, PhaseFlipChannel) {
    PhaseFlipChannel pf(0.2);
    auto result = pf.apply(0);
    EXPECT_GE(result.size(), 1);
}
