#pragma once

#include <vector>
#include <complex>
#include <cstdint>
#include <stdexcept>
#include <cmath>
#include <iostream> // For std::ostream

class StateVector;
// QCSIM_SRS_012: The density matrix backend shall initialize to the pure state |0...0><0...0|.
class DensityMatrix {
public:
    explicit DensityMatrix(size_t n_qubits);
    ~DensityMatrix() = default;

    DensityMatrix(const DensityMatrix&) = delete;
    DensityMatrix& operator=(const DensityMatrix&) = delete;
    DensityMatrix(DensityMatrix&&) = default;
    DensityMatrix& operator=(DensityMatrix&&) = default;

    double get_trace() const;

    /**
     * @brief Applies a quantum gate to the density matrix.
     *
     * This method is now generalized to support multi-qubit gates.
     */
    void apply_gate(const std::vector<std::complex<double>>& gate_matrix, const std::vector<size_t>& target_qubits);

    static DensityMatrix from_statevector(const StateVector& sv);
    
    /**
     * @brief Prints a formatted representation of the density matrix.
     * @param os The output stream to write to (defaults to std::cout).
     */
    void display(std::ostream& os = std::cout) const; // <-- ADDED THIS LINE

    // --- Accessors ---
    size_t get_num_qubits() const { return num_qubits; }
    size_t get_dim() const { return dim; }
    const std::vector<std::complex<double>>& get_matrix() const { return matrix; }
    std::vector<std::complex<double>>& get_matrix() { return matrix; }

private:
    size_t num_qubits;
    size_t dim;
    std::vector<std::complex<double>> matrix;
};
