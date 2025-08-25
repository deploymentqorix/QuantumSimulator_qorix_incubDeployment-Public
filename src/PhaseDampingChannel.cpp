#include "../include/PhaseDampingChannel.hpp"
#include "../include/MatrixUtils.hpp" // Assuming MatrixUtils provides the helpers
#include <vector>
#include <complex>

// NOTE: It's assumed the helper functions like create_full_operator, 
// multiply_matrices, and adjoint are available, for example, in MatrixUtils.

// QCSIM_SRS_111: The simulator shall support a phase-damping (T2) noise channel.
void PhaseDampingChannel::apply(DensityMatrix& dm, size_t target_qubit) const {
    size_t num_qubits = dm.get_num_qubits();
    size_t dim = dm.get_dim();
    auto& rho = dm.get_matrix();

    // Define Kraus operators for phase damping
    std::vector<std::complex<double>> E0_small = {
        {1.0, 0.0}, {0.0, 0.0},
        {0.0, 0.0}, {sqrt(1.0 - probability), 0.0}
    };
    std::vector<std::complex<double>> E1_small = {
        {0.0, 0.0}, {0.0, 0.0},
        {0.0, 0.0}, {sqrt(probability), 0.0}
    };

    auto E0 = MatrixUtils::create_full_operator(E0_small, num_qubits, target_qubit);
    auto E1 = MatrixUtils::create_full_operator(E1_small, num_qubits, target_qubit);
    auto E0_dag = MatrixUtils::adjoint(E0, dim);
    auto E1_dag = MatrixUtils::adjoint(E1, dim);

    auto term1 = MatrixUtils::multiply(E0, MatrixUtils::multiply(rho, E0_dag, dim), dim);
    auto term2 = MatrixUtils::multiply(E1, MatrixUtils::multiply(rho, E1_dag, dim), dim);

    for (size_t i = 0; i < rho.size(); ++i) {
        rho[i] = term1[i] + term2[i];
    }
}
