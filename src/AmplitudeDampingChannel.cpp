#include "AmplitudeDampingChannel.hpp"
#include <vector>
#include <complex>

// --- Helper functions (copied for convenience, consider moving to a common utility file) ---

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

// --- Main apply method ---

void AmplitudeDampingChannel::apply(DensityMatrix& dm, size_t target_qubit) const {
    size_t num_qubits = dm.get_num_qubits();
    size_t dim = dm.get_dim();
    auto& rho = dm.get_matrix();

    // Define Kraus operators for amplitude damping
    std::vector<std::complex<double>> E0_small = {
        {1.0, 0.0}, {0.0, 0.0},
        {0.0, 0.0}, {sqrt(1.0 - probability), 0.0}
    };
    std::vector<std::complex<double>> E1_small = {
        {0.0, 0.0}, {sqrt(probability), 0.0},
        {0.0, 0.0}, {0.0, 0.0}
    };

    auto E0 = create_full_operator(E0_small, num_qubits, target_qubit);
    auto E1 = create_full_operator(E1_small, num_qubits, target_qubit);
    auto E0_dag = adjoint(E0, dim);
    auto E1_dag = adjoint(E1, dim);

    auto term1 = multiply_matrices(E0, multiply_matrices(rho, E0_dag, dim), dim);
    auto term2 = multiply_matrices(E1, multiply_matrices(rho, E1_dag, dim), dim);

    for (size_t i = 0; i < rho.size(); ++i) {
        rho[i] = term1[i] + term2[i];
    }
}
