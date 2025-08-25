#include "../include/DensityMatrix.hpp"
#include "../include/StateVector.hpp"
#include "../include/MatrixUtils.hpp"
#include <stdexcept>
#include <vector>
#include <complex>
#include <iomanip> // For std::setw, std::fixed

// --- Method Implementations ---

// QCSIM_SRS_112: The density matrix backend shall initialize to the pure state |0...0><0...0|.
DensityMatrix::DensityMatrix(size_t n_qubits)
    : num_qubits(n_qubits), dim(1ULL << n_qubits) {
    matrix.resize(dim * dim, {0.0, 0.0});
    matrix[0] = {1.0, 0.0};
}

DensityMatrix DensityMatrix::from_statevector(const StateVector& sv) {
    size_t n_qubits = sv.get_num_qubits();
    DensityMatrix dm(n_qubits);
    const auto& vec = sv.get_state_vector();
    auto& mat = dm.matrix;
    size_t dim = dm.get_dim();
    for (size_t i = 0; i < dim; ++i) {
        for (size_t j = 0; j < dim; ++j) {
            mat[i * dim + j] = vec[i] * std::conj(vec[j]);
        }
    }
    return dm;
}

// QCSIM_SRS_113: The density matrix backend shall apply gates using the transformation rho -> U * rho * U_dag.
void DensityMatrix::apply_gate(
    const std::vector<std::complex<double>>& small_gate_matrix,
    const std::vector<size_t>& target_qubits) {

    size_t num_target_qubits = target_qubits.size();
    size_t small_dim = 1ULL << num_target_qubits;

    if (small_gate_matrix.size() != small_dim * small_dim) {
        throw std::invalid_argument("Gate matrix size does not match number of target qubits.");
    }

    std::vector<std::complex<double>> U(dim * dim, {0.0, 0.0});

    size_t non_target_mask = ~0ULL;
    for (size_t target : target_qubits) {
        non_target_mask &= ~(1ULL << target);
    }

    for (size_t i = 0; i < dim; ++i) {
        size_t non_target_bits = i & non_target_mask;
        size_t small_i = 0;
        for (size_t k = 0; k < num_target_qubits; ++k) {
            if ((i >> target_qubits[k]) & 1) {
                small_i |= (1ULL << k);
            }
        }
        for (size_t j = 0; j < small_dim; ++j) {
            size_t k = non_target_bits;
            for (size_t bit_idx = 0; bit_idx < num_target_qubits; ++bit_idx) {
                if ((j >> bit_idx) & 1) {
                    k |= (1ULL << target_qubits[bit_idx]);
                }
            }
            U[i * dim + k] = small_gate_matrix[small_i * small_dim + j];
        }
    }

    auto U_dagger = MatrixUtils::adjoint(U, dim);
    auto temp_rho = MatrixUtils::multiply(matrix, U_dagger, dim);
    matrix = MatrixUtils::multiply(U, temp_rho, dim);
}

// QCSIM_SRS_115: The density matrix backend shall provide a method to print the final density matrix.
void DensityMatrix::display(std::ostream& os) const {
    os << "Density Matrix (" << dim << "x" << dim << "):\n";
    os << std::fixed << std::setprecision(3);

    for (size_t i = 0; i < dim; ++i) {
        for (size_t j = 0; j < dim; ++j) {
            os << std::setw(5) << matrix[i * dim + j].real() 
               << (matrix[i * dim + j].imag() >= 0 ? "+" : "") 
               << matrix[i * dim + j].imag() << "i ";
        }
        os << "\n";
    }
    os.unsetf(std::ios_base::floatfield); // Reset formatting
}
