#include <gtest/gtest.h>
#include "QuantumCircuit.h"

// Invalid gate addition (e.g., CX with wrong indices)
TEST(CircuitExtended, InvalidGate) {
    QuantumCircuit qc(1);
    EXPECT_THROW(qc.cx(0, 1), std::out_of_range);
}

// Stress test with many gates
TEST(CircuitExtended, StressTest) {
    QuantumCircuit qc(10);
    for (int i = 0; i < 20; i++) {
        qc.h(i % 10);
        qc.x((i+1) % 10);
    }
    EXPECT_EQ(qc.getNumQubits(), 10);
    EXPECT_EQ(qc.getNumGates(), 40);
}
