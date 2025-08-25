#include "DensityMatrixBackend.hpp"
#include "Gates.hpp"
#include <stdexcept>
#include <random>
#include <vector>
#include <variant>
#include <array>

using Matrix2cd = std::array<std::array<std::complex<double>, 2>, 2>;

static std::vector<std::complex<double>> flatten_gate(const Matrix2cd& gate_matrix) {
    return { gate_matrix[0][0], gate_matrix[0][1], gate_matrix[1][0], gate_matrix[1][1] };
}

DensityMatrixBackend::DensityMatrixBackend(uint8_t num_qubits)
    : density_matrix(num_qubits) {}

void DensityMatrixBackend::set_noise_channel(std::shared_ptr<QuantumChannel> channel) {
    noise_channel = std::move(channel);
}

void DensityMatrixBackend::execute(const QuantumCircuit& circuit) {
    density_matrix = DensityMatrix(circuit.get_num_qubits());

    for (const auto& op : circuit.get_ops()) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, GateOp>) {
                if (arg.name == "h") {
                    density_matrix.apply_gate(flatten_gate(Gates::Hadamard), {arg.qubits[0]});
                } else if (arg.name == "x") {
                    density_matrix.apply_gate(flatten_gate(Gates::PauliX), {arg.qubits[0]});
                } else if (arg.name == "cx") {
                    density_matrix.apply_gate(Gates::CNOT, {arg.qubits[0], arg.qubits[1]});
                } else if (arg.name == "cz") {
                    density_matrix.apply_gate(Gates::CZ, {arg.qubits[0], arg.qubits[1]});
                } else if (arg.name == "swap") {
                    density_matrix.apply_gate(Gates::SWAP, {arg.qubits[0], arg.qubits[1]});
                }
                
                if (noise_channel) {
                    for(uint8_t qubit : arg.qubits) {
                        noise_channel->apply(density_matrix, qubit);
                    }
                }
            }
        }, op);
    }
}

static std::vector<double> get_probabilities(const DensityMatrix& dm) {
    size_t dim = dm.get_dim();
    const auto& mat = dm.get_matrix();
    std::vector<double> probs;
    probs.reserve(dim);
    for (size_t i = 0; i < dim; ++i) {
        probs.push_back(mat[i * dim + i].real());
    }
    return probs;
}

std::map<std::string, int> DensityMatrixBackend::get_results(int shots) {
    std::map<std::string, int> counts;
    auto probabilities = get_probabilities(density_matrix);
    uint8_t num_qubits = density_matrix.get_num_qubits();

    static std::mt19937 rng(std::random_device{}());
    std::discrete_distribution<> dist(probabilities.begin(), probabilities.end());

    for (int i = 0; i < shots; ++i) {
        int outcome_decimal = dist(rng);
        std::string outcome_str;
        for (int j = num_qubits - 1; j >= 0; --j) {
            outcome_str += ((outcome_decimal >> j) & 1) ? '1' : '0';
        }
        counts[outcome_str]++;
    }
    return counts;
}

void DensityMatrixBackend::print_state(std::ostream& os) const {
    density_matrix.display(os);
}
#include "../include/DensityMatrixBackend.hpp"
#include "../include/Gates.hpp"
#include <stdexcept>
#include <random>
#include <vector>
#include <variant>
#include <array>

using Matrix2cd = std::array<std::array<std::complex<double>, 2>, 2>;

static std::vector<std::complex<double>> flatten_gate(const Matrix2cd& gate_matrix) {
    return { gate_matrix[0][0], gate_matrix[0][1], gate_matrix[1][0], gate_matrix[1][1] };
}

// QCSIM_SRS_010: The simulator shall have a density matrix backend for noisy simulation.
DensityMatrixBackend::DensityMatrixBackend(uint8_t num_qubits)
    : density_matrix(num_qubits) {}

// QCSIM_SRS_011: The simulator shall support various noise channels, which can be applied after gate operations.
void DensityMatrixBackend::set_noise_channel(std::shared_ptr<QuantumChannel> channel) {
    noise_channel = std::move(channel);
}

void DensityMatrixBackend::execute(const QuantumCircuit& circuit) {
    // QCSIM_SRS_012: The density matrix backend shall initialize to the pure state |0...0><0...0|.
    density_matrix = DensityMatrix(circuit.get_num_qubits());

    for (const auto& op : circuit.get_ops()) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, GateOp>) {
                // QCSIM_SRS_113: The density matrix backend shall apply gates using the transformation rho -> U * rho * U_dag.
                if (arg.name == "h") {
                    density_matrix.apply_gate(flatten_gate(Gates::Hadamard), {arg.qubits[0]});
                } else if (arg.name == "x") {
                    density_matrix.apply_gate(flatten_gate(Gates::PauliX), {arg.qubits[0]});
                } else if (arg.name == "cx") {
                    density_matrix.apply_gate(Gates::CNOT, {arg.qubits[0], arg.qubits[1]});
                } else if (arg.name == "cz") {
                    density_matrix.apply_gate(Gates::CZ, {arg.qubits[0], arg.qubits[1]});
                } else if (arg.name == "swap") {
                    density_matrix.apply_gate(Gates::SWAP, {arg.qubits[0], arg.qubits[1]});
                }
                
                if (noise_channel) {
                    for(uint8_t qubit : arg.qubits) {
                        noise_channel->apply(density_matrix, qubit);
                    }
                }
            }
        }, op);
    }
}

static std::vector<double> get_probabilities(const DensityMatrix& dm) {
    size_t dim = dm.get_dim();
    const auto& mat = dm.get_matrix();
    std::vector<double> probs;
    probs.reserve(dim);
    for (size_t i = 0; i < dim; ++i) {
        probs.push_back(mat[i * dim + i].real());
    }
    return probs;
}

// QCSIM_SRS_030: The simulator shall perform measurements on the density matrix by sampling from the diagonal elements.
std::map<std::string, int> DensityMatrixBackend::get_results(int shots) {
    std::map<std::string, int> counts;
    auto probabilities = get_probabilities(density_matrix);
    uint8_t num_qubits = density_matrix.get_num_qubits();

    static std::mt19937 rng(std::random_device{}());
    std::discrete_distribution<> dist(probabilities.begin(), probabilities.end());

    for (int i = 0; i < shots; ++i) {
        int outcome_decimal = dist(rng);
        std::string outcome_str;
        for (int j = num_qubits - 1; j >= 0; --j) {
            outcome_str += ((outcome_decimal >> j) & 1) ? '1' : '0';
        }
        counts[outcome_str]++;
    }
    return counts;
}

// QCSIM_SRS_015: The density matrix backend shall provide a method to print the final density matrix.
void DensityMatrixBackend::print_state(std::ostream& os) const {
    density_matrix.display(os);
}
