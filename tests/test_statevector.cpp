#include <gtest/gtest.h>
#include "StateVector.h"
#include "QuantumCircuit.h"

// Test Case for QCSIM_SRS_013: StateVector backend shall simulate basic gates
TEST(StateVectorTest, ApplyXGate) {
    StateVector sv(1);
    sv.applyX(0);
    auto result = sv.measureAll();
    EXPECT_EQ(result, 1); // After X gate, |0> -> |1>
}

// Test Case for QCSIM_SRS_010: Bell State
TEST(StateVectorTest, BellState) {
    QuantumCircuit qc(2);
    qc.h(0);
    qc.cx(0, 1);

    StateVector sv(2);
    sv.execute(qc);

    auto probs = sv.probabilities();
    EXPECT_NEAR(probs[0], 0.5, 0.1);
    EXPECT_NEAR(probs[3], 0.5, 0.1);
}
