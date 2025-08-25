#include "../include/StateVectorBackend.hpp"
#include "../include/Gates.hpp"
#include <stdexcept>
#include <random>
#include <vector>
#include <variant>
#include <iostream>

// --- Constructor ---

// QCSIM_SRS_105: The simulator shall have a statevector backend for ideal simulation.
StateVectorBackend::StateVectorBackend(uint8_t num_qubits)
    : state_vector(num_qubits) {}

// --- Simulation Logic ---

void StateVectorBackend::execute(const QuantumCircuit& circuit) {
    // QCSIM_SRS_106: The statevector backend shall initialize to the |0...0> state.
    state_vector = StateVector(circuit.get_num_qubits());

    // QCSIM_SRS_107: The statevector backend shall apply gates by performing state transformations.
    for (const auto& op : circuit.get_ops()) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, GateOp>) {
                // Handle standard single gates
                if (arg.name == "h") {
                    apply_single_qubit(state_vector.get_state_vector_mut(), arg.qubits[0], Gates::Hadamard);
                } else if (arg.name == "x") {
                    apply_single_qubit(state_vector.get_state_vector_mut(), arg.qubits[0], Gates::PauliX);
                } else if (arg.name == "cx") {
                    apply_cnot(state_vector.get_state_vector_mut(), arg.qubits[0], arg.qubits[1]);
                } else if (arg.name == "cz") {
                    apply_cz(state_vector.get_state_vector_mut(), arg.qubits[0], arg.qubits[1]);
                } else if (arg.name == "swap") {
                    apply_swap(state_vector.get_state_vector_mut(), arg.qubits[0], arg.qubits[1]);
                }
            } else if constexpr (std::is_same_v<T, FusedGateOp>) {
                // Handle Fused Gates by applying their inner gates sequentially
                for (const auto& fused_op : arg.ops) {
                    if (fused_op.name == "h") {
                        apply_single_qubit(state_vector.get_state_vector_mut(), fused_op.qubits[0], Gates::Hadamard);
                    } else if (fused_op.name == "x") {
                        apply_single_qubit(state_vector.get_state_vector_mut(), fused_op.qubits[0], Gates::PauliX);
                    }
                    // Add other single-qubit gates here if needed for fusion
                }
            } else if constexpr (std::is_same_v<T, MeasureOp>) {
                // Measurement is handled later in the get_results call, so we do nothing here.
            }
        }, op);
    }
}

// --- Measurement Logic ---

static bool measure_qubit(StateVector& sv, uint8_t qubit) {
    const size_t qubit_mask = 1ULL << qubit;
    double prob_of_one = 0.0;
    const auto& vec = sv.get_state_vector();
    for (size_t k = 0; k < vec.size(); ++k) {
        if ((k & qubit_mask) != 0) {
            prob_of_one += std::norm(vec[k]);
        }
    }
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return (dist(rng) < prob_of_one);
}

// QCSIM_SRS_128: The simulator shall perform measurements based on the probabilities derived from the state vector amplitudes.
std::map<std::string, int> StateVectorBackend::get_results(int shots) {
    std::map<std::string, int> counts;
    uint8_t num_cbits = state_vector.get_num_qubits(); 

    for (int i = 0; i < shots; ++i) {
        std::vector<int> classical_bits(num_cbits, 0);
        for (uint8_t j = 0; j < num_cbits; ++j) {
            classical_bits[j] = measure_qubit(state_vector, j);
        }

        std::string outcome_str;
        for (int j = classical_bits.size() - 1; j >= 0; --j) {
            outcome_str += std::to_string(classical_bits[j]);
        }
        counts[outcome_str]++;
    }
    return counts;
}

// --- State Printing ---
// QCSIM_SRS_109: The statevector backend shall provide a method to print the final state vector.
void StateVectorBackend::print_state(std::ostream& os) const {
    state_vector.display(os);
}
