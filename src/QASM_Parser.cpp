#include "QASM_Parser.hpp"
#include <sstream>
#include <stdexcept>
#include <vector>
#include <charconv> // For std::from_chars

// Helper function to trim whitespace
static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

// QCSIM_SRS_114: A QASM_Parser class shall be implemented.
QuantumCircuit QASM_Parser::parse(const std::string& qasm_content) {
    std::stringstream stream(qasm_content);
    std::string line;

    uint8_t num_qubits = 0;
    uint8_t num_cbits = 0;

    // First pass to find register declarations
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.rfind("qreg", 0) == 0) {
            // Example: qreg q[2];
            size_t start = line.find('[') + 1;
            size_t end = line.find(']');
            std::string num_str = line.substr(start, end - start);
            std::from_chars(num_str.data(), num_str.data() + num_str.size(), num_qubits);
        } else if (line.rfind("creg", 0) == 0) {
            size_t start = line.find('[') + 1;
            size_t end = line.find(']');
            std::string num_str = line.substr(start, end - start);
            std::from_chars(num_str.data(), num_str.data() + num_str.size(), num_cbits);
        }
    }

    if (num_qubits == 0) {
        throw std::runtime_error("QASM parsing error: No quantum registers (qreg) declared.");
    }

    QuantumCircuit circuit(num_qubits, num_cbits);
    
    // Second pass for operations
    stream.clear();
    stream.seekg(0);

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line.rfind("//", 0) == 0 || line.rfind("OPENQASM", 0) == 0 || line.rfind("include", 0) == 0 || line.rfind("qreg", 0) == 0 || line.rfind("creg", 0) == 0) {
            continue; // Skip comments, headers, and declarations
        }

        std::stringstream line_stream(line);
        std::string op_name;
        line_stream >> op_name;

        if (op_name == "measure") {
            // measure q[0] -> c[0];
            std::string q_arg, arrow, c_arg;
            line_stream >> q_arg >> arrow >> c_arg;
            
            uint8_t q_idx = static_cast<uint8_t>(std::stoi(q_arg.substr(q_arg.find('[') + 1)));
            uint8_t c_idx = static_cast<uint8_t>(std::stoi(c_arg.substr(c_arg.find('[') + 1)));
            circuit.add_op(MeasureOp{q_idx, c_idx});

        } else { // It's a gate
            std::string rest_of_line;
            std::getline(line_stream, rest_of_line);
            rest_of_line = trim(rest_of_line);
            
            std::vector<uint8_t> qubits;
            size_t start = 0;
            while((start = rest_of_line.find("q[", start)) != std::string::npos) {
                size_t end = rest_of_line.find(']', start);
                std::string num_str = rest_of_line.substr(start + 2, end - (start + 2));
                qubits.push_back(static_cast<uint8_t>(std::stoi(num_str)));
                start = end + 1;
            }
            circuit.add_op(GateOp{op_name, qubits});
        }
    }

    return circuit;
}
