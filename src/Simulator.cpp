#include "../include/Simulator.hpp"
#include <stdexcept>

// QCSIM_SRS_101: The simulator shall provide a main class to manage the simulation backend and execute circuits.
Simulator::Simulator(std::unique_ptr<SimulationBackend> backend)
    : sim_backend(std::move(backend)) {
    if (!sim_backend) {
        throw std::invalid_argument("Simulator backend cannot be null.");
    }
}

Simulator::~Simulator() = default;

// QCSIM_SRS_129: The simulator shall support a configurable number of shots for measurement statistics.
std::map<std::string, int> Simulator::execute(const QuantumCircuit& circuit, int shots) {
    sim_backend->execute(circuit);
    return sim_backend->get_results(shots);
}

void Simulator::print_final_state(std::ostream& os) const {
    sim_backend->print_state(os);
}
