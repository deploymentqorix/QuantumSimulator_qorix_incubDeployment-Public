#pragma once

#include "SimulationBackend.hpp"
#include "StateVector.hpp"
#include <memory>
#include <iostream> // For std::ostream

class StateVectorBackend : public SimulationBackend {
public:
    explicit StateVectorBackend(uint8_t num_qubits);

    void execute(const QuantumCircuit& circuit) override;
    std::map<std::string, int> get_results(int shots) override;

    // ADD THIS LINE to declare the print_state method
    void print_state(std::ostream& os = std::cout) const override;

private:
    StateVector state_vector;
};
