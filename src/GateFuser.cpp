#include "../include/GateFuser.hpp"
#include <vector>
#include <iostream>

QuantumCircuit GateFuser::optimize(const QuantumCircuit& circuit) {
    std::vector<CircuitOp> original_ops = circuit.get_ops();
    std::vector<CircuitOp> fused_ops;
    
    for (size_t i = 0; i < original_ops.size(); ) {
        const auto& op = original_ops[i];

        // Check if the current op is a single-qubit gate
        if (std::holds_alternative<GateOp>(op)) {
            const auto& gate = std::get<GateOp>(op);
            if (gate.qubits.size() == 1) {
                
                // Start a potential fusion group
                std::vector<GateOp> fusion_group;
                fusion_group.push_back(gate);
                
                uint8_t target_qubit = gate.qubits[0];
                size_t j = i + 1;

                // Look ahead to find more single-qubit gates on the same qubit
                while (j < original_ops.size() && std::holds_alternative<GateOp>(original_ops[j])) {
                    const auto& next_gate = std::get<GateOp>(original_ops[j]);
                    if (next_gate.qubits.size() == 1 && next_gate.qubits[0] == target_qubit) {
                        fusion_group.push_back(next_gate);
                        j++;
                    } else {
                        break; // Stop if it's a multi-qubit gate or on a different qubit
                    }
                }

                // If we found more than one gate to fuse, create a FusedGateOp
                if (fusion_group.size() > 1) {
                    fused_ops.push_back(FusedGateOp{fusion_group, {target_qubit}});
                    i = j; // Move the main index past the fused gates
                    continue;
                }
            }
        }
        
        // If no fusion happened, just add the original op and move to the next one
        fused_ops.push_back(op);
        i++;
    }

    // Create a new circuit with the optimized operations
    QuantumCircuit optimized_circuit(circuit.get_num_qubits(), circuit.get_num_cbits());
    optimized_circuit.set_ops(fused_ops);
    
    return optimized_circuit;
}
