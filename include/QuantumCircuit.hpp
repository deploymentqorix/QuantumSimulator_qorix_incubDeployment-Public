#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <variant>
#include <ostream>
#include <iostream>

// QCSIM_SRS_103: Define structures for gate operations
struct GateOp {
    std::string name;
    std::vector<uint8_t> qubits;
};

// NEW: Define a structure for a fused gate operation
struct FusedGateOp {
    std::vector<GateOp> ops; // The sequence of gates that were fused
    std::vector<uint8_t> qubits; // The qubits this fused gate acts on
};

// QCSIM_SRS_127: Define structure for measurement operations
struct MeasureOp {
    uint8_t qubit;
    uint8_t classical_bit;
};

// Update CircuitOp to include the new FusedGateOp
using CircuitOp = std::variant<GateOp, MeasureOp, FusedGateOp>;

// QCSIM_SRS_101: A QuantumCircuit class to store a sequence of operations.
class QuantumCircuit {
public:
    // QCSIM_SRS_102: Manage the number of qubits for the circuit.
    QuantumCircuit(uint8_t n_qubits, uint8_t n_cbits);

    void add_op(const CircuitOp& op);
    
    uint8_t get_num_qubits() const { return num_qubits; }
    uint8_t get_num_cbits() const { return num_cbits; }
    const std::vector<CircuitOp>& get_ops() const { return operations; }
    
    // NEW: A method to replace the circuit's operations after fusion
    void set_ops(const std::vector<CircuitOp>& new_ops) { operations = new_ops; }


    // QCSIM_SRS_108: A display method to print the circuit.
    void display(std::ostream& os = std::cout) const;

private:
    uint8_t num_qubits;
    uint8_t num_cbits;
    std::vector<CircuitOp> operations;
};
