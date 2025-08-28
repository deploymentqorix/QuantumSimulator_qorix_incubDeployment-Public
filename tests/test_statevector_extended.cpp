#include <gtest/gtest.h>
#include "StateVector.h"
#include "QuantumCircuit.h"

// GHZ State: |000> + |111>
TEST(StateVectorExtended, GHZState) {
    QuantumCircuit qc(3);
    qc.h(0);
    qc.cx(0, 1);
    qc.cx(0, 2);

    StateVector sv(3);
    sv.execute(qc);
    auto probs = sv.probabilities();
    EXPECT_NEAR(probs[0], 0.5, 0.1);
    EXPECT_NEAR(probs[7], 0.5, 0.1);
}

// Invalid qubit index handling
TEST(StateVectorExtended, InvalidQubitIndex) {
    StateVector sv(2);
    EXPECT_THROW(sv.applyX(5), std::out_of_range);
}

// Measurement repeatability (statistical test)
TEST(StateVectorExtended, MeasurementRepeatability) {
    QuantumCircuit qc(1);
    qc.h(0);
    StateVector sv(1);
    sv.execute(qc);

    int shots = 1000;
    int ones = 0;
    for (int i = 0; i < shots; i++) {
        ones += sv.measureAll();
    }
    double ratio = double(ones) / shots;
    EXPECT_NEAR(ratio, 0.5, 0.1);
}
