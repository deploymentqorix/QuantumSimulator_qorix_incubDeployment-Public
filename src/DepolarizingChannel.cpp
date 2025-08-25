#include "../include/DepolarizingChannel.hpp"
#include "../include/MatrixUtils.hpp"
#include <vector>
#include <complex>

// NOTE: It's assumed the helper functions like create_full_operator
// are available, for example, in MatrixUtils.

// QCSIM_SRS_111: The simulator shall support a depolarizing noise channel.
void DepolarizingChannel::apply(DensityMatrix& dm, size_t target_qubit) const {
    size_t n_qubits = dm.get_num_qubits();
    size_t dim = dm.get_dim();
    auto& rho = dm.get_matrix();

    double p_3 = probability / 3.0;
    // Kraus operators for Depolarizing Channel
    std::vector<std::complex<double>> E0_small = {{sqrt(1.0 - probability), 0}, {0, 0}, {0, 0}, {sqrt(1.0 - probability), 0}}; // sqrt(1-p) * I
    std::vector<std::complex<double>> E1_small = {{0, 0}, {sqrt(p_3), 0}, {sqrt(p_3), 0}, {0, 0}}; // sqrt(p/3) * X
    std::vector<std::complex<double>> E2_small = {{0, 0}, {0, -sqrt(p_3)}, {0, sqrt(p_3)}, {0, 0}}; // sqrt(p/3) * Y
    std::vector<std::complex<double>> E3_small = {{sqrt(p_3), 0}, {0, 0}, {0, 0}, {-sqrt(p_3), 0}}; // sqrt(p/3) * Z

    auto E0 = MatrixUtils::create_full_operator(E0_small, n_qubits, target_qubit);
    auto E1 = MatrixUtils::create_full_operator(E1_small, n_qubits, target_qubit);
    auto E2 = MatrixUtils::create_full_operator(E2_small, n_qubits, target_qubit);
    auto E3 = MatrixUtils::create_full_operator(E3_small, n_qubits, target_qubit);

    auto E0_dag = MatrixUtils::adjoint(E0, dim);
    auto E1_dag = MatrixUtils::adjoint(E1, dim);
    auto E2_dag = MatrixUtils::adjoint(E2, dim);
    auto E3_dag = MatrixUtils::adjoint(E3, dim);

    auto term0 = MatrixUtils::multiply(E0, MatrixUtils::multiply(rho, E0_dag, dim), dim);
    auto term1 = MatrixUtils::multiply(E1, MatrixUtils::multiply(rho, E1_dag, dim), dim);
    auto term2 = MatrixUtils::multiply(E2, MatrixUtils::multiply(rho, E2_dag, dim), dim);
    auto term3 = MatrixUtils::multiply(E3, MatrixUtils::multiply(rho, E3_dag, dim), dim);

    for (size_t i = 0; i < rho.size(); ++i) {
        rho[i] = term0[i] + term1[i] + term2[i] + term3[i];
    }
}
