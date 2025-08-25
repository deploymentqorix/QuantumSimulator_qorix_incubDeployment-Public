#pragma once

#include "QuantumCircuit.hpp"
#include <string>

// QCSIM_SRS_014: A QASM_Parser class shall be implemented.
class QASM_Parser {
public:
    /**
     * @brief Parses a QASM string and returns a QuantumCircuit object.
     * @param qasm_content The string containing the QASM 2.0 circuit.
     * @return A QuantumCircuit object.
     */
    QuantumCircuit parse(const std::string& qasm_content);
};