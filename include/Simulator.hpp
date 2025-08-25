#pragma once

#include "QuantumCircuit.hpp"
#include "SimulationBackend.hpp"
#include <map>
#include <string>
#include <memory>

class Simulator {
public:
    explicit Simulator(std::unique_ptr<SimulationBackend> backend);
    ~Simulator(); // <-- ADD THIS LINE

    std::map<std::string, int> execute(const QuantumCircuit& circuit, int shots);
    void print_final_state(std::ostream& os = std::cout) const;

private:
    std::unique_ptr<SimulationBackend> sim_backend;
};
