#include "PhaseFlipChannel.hpp"
#include "MatrixUtils.hpp"
#include <vector>
#include <complex>

// Local helper function
static std::vector<std::complex<double>> create_full_operator(
    const std::vector<std::complex<double>>& small_op, size_t n_qubits, size_t target) {
    size_t dim = 1ULL << n_qubits;
    std::vector<std::complex<double>> full_op(dim * dim, {0.0, 0.0});
    for (size_t i = 0; i < dim; ++i) {
        size_t i_bit = (i >> target) & 1;
        for (size_t j = 0; j < dim; ++j) {
            size_t j_bit = (j >> target) & 1;
            if (((i ^ j) & ~((size_t)1 << target)) == 0) {
                full_op[i * dim + j] = small_op[i_bit * 2 + j_bit];
            }
        }
    }
    return full_op;
}
// QCSIM_SRS_111: The simulator shall support a phase-flip noise channel.
void PhaseFlipChannel::apply(DensityMatrix& dm, size_t target_qubit) const {
    size_t n_qubits = dm.get_num_qubits();
    size_t dim = dm.get_dim();
    auto& rho = dm.get_matrix();

    // Kraus operators for Phase Flip: E0 = sqrt(1-p) * I, E1 = sqrt(p) * Z
    std::vector<std::complex<double>> E0_small = {{sqrt(1.0 - probability), 0}, {0, 0}, {0, 0}, {sqrt(1.0 - probability), 0}};
    std::vector<std::complex<double>> E1_small = {{sqrt(probability), 0}, {0, 0}, {0, 0}, {-sqrt(probability), 0}};

    auto E0 = create_full_operator(E0_small, n_qubits, target_qubit);
    auto E1 = create_full_operator(E1_small, n_qubits, target_qubit);

    auto E0_dag = MatrixUtils::adjoint(E0, dim);
    auto E1_dag = MatrixUtils::adjoint(E1, dim);
    auto term1 = MatrixUtils::multiply(E0, MatrixUtils::multiply(rho, E0_dag, dim), dim);
    auto term2 = MatrixUtils::multiply(E1, MatrixUtils::multiply(rho, E1_dag, dim), dim);

    for (size_t i = 0; i < rho.size(); ++i) {
        rho[i] = term1[i] + term2[i];
    }
}