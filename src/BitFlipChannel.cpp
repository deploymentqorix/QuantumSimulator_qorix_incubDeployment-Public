#include "BitFlipChannel.hpp"
#include "Gates.hpp" // For PauliX matrix
#include <vector>
#include <complex>

// Helper function to create the full (dim x dim) operator for a single qubit.
// This is very similar to the apply_gate logic in DensityMatrix.cpp.
static std::vector<std::complex<double>> create_full_operator(
    const std::vector<std::complex<double>>& small_op,
    size_t num_qubits,
    size_t target_qubit) {
    
    size_t dim = 1ULL << num_qubits;
    std::vector<std::complex<double>> full_op(dim * dim, {0.0, 0.0});

    for (size_t i = 0; i < dim; ++i) {
        size_t i_bit = (i >> target_qubit) & 1;
        for (size_t j = 0; j < dim; ++j) {
            size_t j_bit = (j >> target_qubit) & 1;
            
            bool non_target_bits_match = ((i ^ j) & ~((size_t)1 << target_qubit)) == 0;

            if (non_target_bits_match) {
                full_op[i * dim + j] = small_op[i_bit * 2 + j_bit];
            }
        }
    }
    return full_op;
}

// Helper for matrix multiplication: C = A * B
static std::vector<std::complex<double>> multiply_matrices(
    const std::vector<std::complex<double>>& A,
    const std::vector<std::complex<double>>& B,
    size_t dim) {
    std::vector<std::complex<double>> C(dim * dim, {0.0, 0.0});
    for (size_t i = 0; i < dim; ++i) {
        for (size_t j = 0; j < dim; ++j) {
            for (size_t k = 0; k < dim; ++k) {
                C[i * dim + j] += A[i * dim + k] * B[k * dim + j];
            }
        }
    }
    return C;
}

// Helper for conjugate transpose: A†
static std::vector<std::complex<double>> adjoint(
    const std::vector<std::complex<double>>& A,
    size_t dim) {
    std::vector<std::complex<double>> A_adj(dim * dim);
    for (size_t i = 0; i < dim; ++i) {
        for (size_t j = 0; j < dim; ++j) {
            A_adj[i * dim + j] = std::conj(A[j * dim + i]);
        }
    }
    return A_adj;
}


// QCSIM_SRS_150: Apply channel using operator-sum representation.
void BitFlipChannel::apply(DensityMatrix& dm, size_t target_qubit) const {
    size_t num_qubits = dm.get_num_qubits();
    size_t dim = dm.get_dim();
    auto& rho = dm.get_matrix(); // Get a mutable reference to the density matrix data

    // Define Kraus operators for the bit-flip channel
    // E0 = sqrt(1-p) * I
    std::vector<std::complex<double>> E0_small = {
        {sqrt(1.0 - probability), 0.0}, {0.0, 0.0},
        {0.0, 0.0}, {sqrt(1.0 - probability), 0.0}
    };

    // E1 = sqrt(p) * X
    std::vector<std::complex<double>> E1_small = {
        {0.0, 0.0}, {sqrt(probability), 0.0},
        {sqrt(probability), 0.0}, {0.0, 0.0}
    };

    // Construct full-size Kraus operators
    auto E0 = create_full_operator(E0_small, num_qubits, target_qubit);
    auto E1 = create_full_operator(E1_small, num_qubits, target_qubit);

    auto E0_dag = adjoint(E0, dim);
    auto E1_dag = adjoint(E1, dim);

    // Calculate term 1: E0 * ρ * E0†
    auto term1_temp = multiply_matrices(rho, E0_dag, dim);
    auto term1 = multiply_matrices(E0, term1_temp, dim);

    // Calculate term 2: E1 * ρ * E1†
    auto term2_temp = multiply_matrices(rho, E1_dag, dim);
    auto term2 = multiply_matrices(E1, term2_temp, dim);

    // The new density matrix is the sum of the terms
    for (size_t i = 0; i < rho.size(); ++i) {
        rho[i] = term1[i] + term2[i];
    }
}
