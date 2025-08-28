#include <gtest/gtest.h>
#include "DepolarizingChannel.h"
#include "PhaseFlipChannel.h"

// Chaining channels
TEST(NoiseExtended, ChainChannels) {
    DepolarizingChannel dc(0.1);
    PhaseFlipChannel pf(0.2);
    auto res1 = dc.apply(0);
    auto res2 = pf.apply(0);
    EXPECT_GE(res1.size(), 1);
    EXPECT_GE(res2.size(), 1);
}

// Probabilities normalization check
TEST(NoiseExtended, Normalization) {
    DepolarizingChannel dc(0.3);
    auto probs = dc.apply(0);
    double sum = 0;
    for (auto &p : probs) sum += p;
    EXPECT_NEAR(sum, 1.0, 1e-6);
}
