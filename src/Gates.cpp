#include "Gates.hpp"
#include "StateVector.hpp"
#include <stdexcept>
#include <omp.h>
#include <complex>
#include <vector>
#include <algorithm> // For std::swap

// --- Gate Application Implementations ---

// QCSIM_SRS_103: The simulator shall define a set of standard quantum gates (e.g., H, X, CNOT, CZ, SWAP).
// QCSIM_SRS_124: The simulator shall utilize multi-threading with OpenMP to parallelize computationally intensive loops.
void apply_single_qubit(StateVector& sv, uint8_t qubit, const Matrix2cd& gate) {
    auto& vec = sv.get_state_vector_mut();
    size_t dim = vec.size();
    size_t k = 1ULL << qubit;

    #pragma omp parallel for
    for (size_t i = 0; i < dim; i += 2 * k) {
        for (size_t j = 0; j < k; ++j) {
            size_t i0 = i + j;
            size_t i1 = i + j + k;
            
            std::complex<double> v0 = vec[i0];
            std::complex<double> v1 = vec[i1];

            vec[i0] = gate[0][0] * v0 + gate[0][1] * v1;
            vec[i1] = gate[1][0] * v0 + gate[1][1] * v1;
        }
    }
}

// QCSIM_SRS_103: The simulator shall define a set of standard quantum gates (e.g., H, X, CNOT, CZ, SWAP).
// QCSIM_SRS_124: The simulator shall utilize multi-threading with OpenMP to parallelize computationally intensive loops.
void apply_cnot(StateVector& sv, uint8_t control, uint8_t target) {
    auto& vec = sv.get_state_vector_mut();
    size_t dim = vec.size();
    const size_t control_mask = 1ULL << control;
    const size_t target_mask = 1ULL << target;

    #pragma omp parallel for
    for (size_t i = 0; i < dim; ++i) {
        // Apply X to target if control is |1>
        if ((i & control_mask) != 0) {
            // Find the paired state index by flipping the target bit
            size_t j = i ^ target_mask;
            // Swap only for one of the pairs to avoid swapping back
            if (i < j) {
                std::swap(vec[i], vec[j]);
            }
        }
    }
}

// QCSIM_SRS_103: The simulator shall define a set of standard quantum gates (e.g., H, X, CNOT, CZ, SWAP).
// QCSIM_SRS_124: The simulator shall utilize multi-threading with OpenMP to parallelize computationally intensive loops.
void apply_cz(StateVector& sv, uint8_t control, uint8_t target) {
    auto& vec = sv.get_state_vector_mut();
    size_t dim = vec.size();
    const size_t control_mask = 1ULL << control;
    const size_t target_mask = 1ULL << target;

    #pragma omp parallel for
    for (size_t i = 0; i < dim; ++i) {
        if ((i & control_mask) && (i & target_mask)) {
            vec[i] *= -1.0;
        }
    }
}

// QCSIM_SRS_103: The simulator shall define a set of standard quantum gates (e.g., H, X, CNOT, CZ, SWAP).
// QCSIM_SRS_124: The simulator shall utilize multi-threading with OpenMP to parallelize computationally intensive loops.
void apply_swap(StateVector& sv, uint8_t qubit1, uint8_t qubit2) {
    auto& vec = sv.get_state_vector_mut();
    size_t dim = vec.size();
    const size_t q1_mask = 1ULL << qubit1;
    const size_t q2_mask = 1ULL << qubit2;

    #pragma omp parallel for
    for (size_t i = 0; i < dim; ++i) {
        bool q1_is_1 = (i & q1_mask) != 0;
        bool q2_is_1 = (i & q2_mask) != 0;
        if (q1_is_1 != q2_is_1) {
            size_t j = i ^ (q1_mask | q2_mask);
            if (i < j) {
                std::swap(vec[i], vec[j]);
            }
        }
    }
}
