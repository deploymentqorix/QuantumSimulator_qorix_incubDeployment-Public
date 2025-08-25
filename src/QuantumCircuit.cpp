#include "../include/QuantumCircuit.hpp"
#include <iostream>
#include <stdexcept>

// QCSIM_SRS_102: The circuit shall be initialized with a specific number of qubits and classical bits.
QuantumCircuit::QuantumCircuit(uint8_t n_qubits, uint8_t n_cbits)
    : num_qubits(n_qubits), num_cbits(n_cbits) {
    // Constructor body is empty as members are initialized above
}

// Adds a gate or measurement operation to the circuit
void QuantumCircuit::add_op(const CircuitOp& op) {
    operations.push_back(op);
}

// QCSIM_SRS_108: The QuantumCircuit class shall provide a method to display the circuit's operations.
void QuantumCircuit::display(std::ostream& os) const {
    os << "Quantum Circuit (" << static_cast<int>(num_qubits) << " qubits, "
       << static_cast<int>(num_cbits) << " classical bits):" << std::endl;

    for (const auto& op : operations) {
        std::visit([&os](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, GateOp>) {
                os << "  Gate: " << arg.name << " on qubit(s): ";
                for (size_t i = 0; i < arg.qubits.size(); ++i) {
                    os << static_cast<int>(arg.qubits[i]) << (i == arg.qubits.size() - 1 ? "" : ", ");
                }
                os << std::endl;
            } else if constexpr (std::is_same_v<T, MeasureOp>) {
                os << "  Measure: qubit " << static_cast<int>(arg.qubit)
                   << " -> classical bit " << static_cast<int>(arg.classical_bit) << std::endl;
            } else if constexpr (std::is_same_v<T, FusedGateOp>) {
                // Handle printing for FusedGateOp
                os << "  Fused Gate on qubit(s): ";
                 for (size_t i = 0; i < arg.qubits.size(); ++i) {
                    os << static_cast<int>(arg.qubits[i]) << (i == arg.qubits.size() - 1 ? "" : ", ");
                }
                os << " {" << std::endl;
                for (const auto& fused_op : arg.ops) {
                    os << "    - " << fused_op.name << std::endl;
                }
                os << "  }" << std::endl;
            }
        }, op);
    }
}
