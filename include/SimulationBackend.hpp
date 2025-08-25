#pragma once

#include "QuantumCircuit.hpp"
#include <map>
#include <string>
#include <vector>

/**
 * @class SimulationBackend
 * @brief An abstract base class defining the interface for all simulation backends.
 *
 * This allows the main Simulator class to be agnostic to the underlying
 * simulation method (e.g., state vector vs. density matrix).
 *
 * QCSIM_SRS_164: The Simulator class shall be refactored to support different backends.
 */
class SimulationBackend {
public:
    virtual ~SimulationBackend() = default;

    // Executes the full circuit.
    virtual void execute(const QuantumCircuit& circuit) = 0;

    // Performs measurements and returns the results.
    virtual std::map<std::string, int> get_results(int shots) = 0;
    virtual void print_state(std::ostream& os = std::cout) const = 0;
};
