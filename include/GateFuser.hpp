#pragma once

#include "QuantumCircuit.hpp"

// QCSIM_SRS_078: A GateFuser class for optimizing circuits.
class GateFuser {
public:
    // Takes a quantum circuit and returns a new circuit with fused gates.
    static QuantumCircuit optimize(const QuantumCircuit& circuit);
};
