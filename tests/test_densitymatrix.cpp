#include <gtest/gtest.h>
#include "DensityMatrix.h"
#include "QuantumCircuit.h"

// QCSIM_SRS_014: DensityMatrix backend simulation
TEST(DensityMatrixTest, BasicHadamard) {
    QuantumCircuit qc(1);
    qc.h(0);

    DensityMatrix dm(1);
    dm.execute(qc);

    auto probs = dm.probabilities();
    EXPECT_NEAR(probs[0], 0.5, 0.1);
    EXPECT_NEAR(probs[1], 0.5, 0.1);
}
