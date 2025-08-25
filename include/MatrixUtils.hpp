#pragma once

#include <vector>
#include <complex>

namespace MatrixUtils {

/**
 * @brief Performs optimized, parallel matrix multiplication: C = A * B.
 *
 * This implementation is parallelized with OpenMP and vectorized with AVX2.
 *
 * @param A The first matrix.
 * @param B The second matrix.
 * @param dim The dimension of the matrices.
 * @return The resulting matrix C.
 */
std::vector<std::complex<double>> multiply(
    const std::vector<std::complex<double>>& A,
    const std::vector<std::complex<double>>& B,
    size_t dim);

/**
 * @brief Computes the conjugate transpose (adjoint) of a matrix: A†.
 *
 * @param A The matrix to find the adjoint of.
 * @param dim The dimension of the matrix.
 * @return The resulting adjoint matrix A†.
 */
std::vector<std::complex<double>> adjoint(
    const std::vector<std::complex<double>>& A,
    size_t dim);

} // namespace MatrixUtils
