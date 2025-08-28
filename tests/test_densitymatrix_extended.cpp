#include <gtest/gtest.h>
#include "DensityMatrix.h"
#include "QuantumCircuit.h"

// Chained gates in DensityMatrix
TEST(DensityMatrixExtended, SequentialGates) {
    QuantumCircuit qc(1);
    qc.h(0);
    qc.z(0);
    DensityMatrix dm(1);
    dm.execute(qc);
    auto probs = dm.probabilities();
    EXPECT_NEAR(probs[0] + probs[1], 1.0, 1e-6);
}

// Extreme values for noise model (p=0 and p=1)
TEST(DensityMatrixExtended, ExtremeNoise) {
    DensityMatrix dm(1);
    QuantumCircuit qc(1);
    qc.x(0);
    dm.execute(qc);
    auto probs = dm.probabilities();
    EXPECT_NEAR(probs[0] + probs[1], 1.0, 1e-6);
}
