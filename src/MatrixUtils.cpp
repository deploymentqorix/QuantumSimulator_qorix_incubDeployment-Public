#include "MatrixUtils.hpp"
#include <immintrin.h> // Required for AVX2 intrinsics
#include <omp.h>       // Required for OpenMP

namespace MatrixUtils {

// QCSIM_SRS_147, QCSIM_SRS_148: Optimized with AVX2 and OpenMP
std::vector<std::complex<double>> multiply(
    const std::vector<std::complex<double>>& A,
    const std::vector<std::complex<double>>& B,
    size_t dim) {
    
    std::vector<std::complex<double>> C(dim * dim, {0.0, 0.0});

    // Parallelize the outer loop with OpenMP. This will create multiple threads.
    #pragma omp parallel for
    for (size_t i = 0; i < dim; ++i) {
        for (size_t k = 0; k < dim; ++k) {
            // Load the complex number from A into a SIMD register and broadcast it.
            // This prepares it for multiplication with multiple elements from B.
            __m256d a_broadcast = _mm256_set_pd(A[i * dim + k].imag(), A[i * dim + k].real(), A[i * dim + k].imag(), A[i * dim + k].real());
            
            // The inner loop is vectorized using AVX2.
            // We process 2 complex numbers (4 doubles) at a time.
            for (size_t j = 0; j < dim; j += 2) {
                // Load 2 complex numbers from B
                __m256d b_vals = _mm256_loadu_pd(reinterpret_cast<const double*>(&B[k * dim + j]));
                
                // Load 2 complex numbers from our result matrix C
                __m256d c_vals = _mm256_loadu_pd(reinterpret_cast<const double*>(&C[i * dim + j]));

                // This is the core AVX2 complex multiplication instruction.
                // It performs (a.real*b.real - a.imag*b.imag, a.real*b.imag + a.imag*b.real)
                // for two complex numbers at once.
                __m256d temp = _mm256_fmaddsub_pd(a_broadcast, b_vals, _mm256_mul_pd(_mm256_permute_pd(a_broadcast, 0b0101), _mm256_permute_pd(b_vals, 0b0101)));
                
                // Add the result to the values from C
                __m256d result = _mm256_add_pd(c_vals, temp);

                // Store the final 2 complex numbers back into the result matrix C
                _mm256_storeu_pd(reinterpret_cast<double*>(&C[i * dim + j]), result);
            }
        }
    }
    return C;
}


std::vector<std::complex<double>> adjoint(
    const std::vector<std::complex<double>>& A,
    size_t dim) {
    
    std::vector<std::complex<double>> A_adj(dim * dim);
    // Parallelize this operation with OpenMP for another speed boost.
    #pragma omp parallel for
    for (size_t i = 0; i < dim; ++i) {
        for (size_t j = 0; j < dim; ++j) {
            A_adj[i * dim + j] = std::conj(A[j * dim + i]);
        }
    }
    return A_adj;
}

} // namespace MatrixUtils
