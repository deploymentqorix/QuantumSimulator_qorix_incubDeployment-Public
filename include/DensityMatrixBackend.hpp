#pragma once

#include "SimulationBackend.hpp"
#include "DensityMatrix.hpp"
#include "QuantumChannel.hpp"
#include <vector>
#include <memory>
#include <string>
#include <iostream> // For std::ostream

class DensityMatrixBackend : public SimulationBackend {
public:
    explicit DensityMatrixBackend(uint8_t num_qubits);

    void execute(const QuantumCircuit& circuit) override;
    std::map<std::string, int> get_results(int shots) override;

    // ADD THIS LINE to declare the print_state method
    void print_state(std::ostream& os = std::cout) const override;

    void set_noise_channel(std::shared_ptr<QuantumChannel> channel);

private:
    DensityMatrix density_matrix;
    std::shared_ptr<QuantumChannel> noise_channel;
};
