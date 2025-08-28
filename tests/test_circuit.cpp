#include <gtest/gtest.h>
#include "QuantumCircuit.h"

// QCSIM_SRS_009: QuantumCircuit supports multiple gates
TEST(CircuitTest, AddGates) {
    QuantumCircuit qc(2);
    qc.h(0);
    qc.cx(0, 1);

    EXPECT_EQ(qc.getNumQubits(), 2);
    EXPECT_GT(qc.getNumGates(), 0);
}
