#pragma once

#include <complex>
#include <array>
#include <vector>
#include <cmath>      // For sqrt
#include <cstdint>    // Defines uint8_t

// Define a type for a 2x2 complex matrix
using Matrix2cd = std::array<std::array<std::complex<double>, 2>, 2>;

namespace Gates {
    // --- Single-Qubit Gates (Corrected Initializer Syntax) ---
    const Matrix2cd Hadamard = {{
        {std::complex<double>(1/sqrt(2), 0), std::complex<double>(1/sqrt(2), 0)},
        {std::complex<double>(1/sqrt(2), 0), std::complex<double>(-1/sqrt(2), 0)}
    }};
    const Matrix2cd PauliX = {{
        {std::complex<double>(0, 0), std::complex<double>(1, 0)},
        {std::complex<double>(1, 0), std::complex<double>(0, 0)}
    }};
    const Matrix2cd PauliZ = {{
        {std::complex<double>(1, 0), std::complex<double>(0, 0)},
        {std::complex<double>(0, 0), std::complex<double>(-1, 0)}
    }};

    // --- Multi-Qubit Gates (Flattened for Density Matrix) ---
    const std::vector<std::complex<double>> CNOT = {
        {1,0}, {0,0}, {0,0}, {0,0},
        {0,0}, {1,0}, {0,0}, {0,0},
        {0,0}, {0,0}, {0,0}, {1,0},
        {0,0}, {0,0}, {1,0}, {0,0}
    };
    
    const std::vector<std::complex<double>> CZ = {
        {1,0}, {0,0}, {0,0}, {0,0},
        {0,0}, {1,0}, {0,0}, {0,0},
        {0,0}, {0,0}, {1,0}, {0,0},
        {0,0}, {0,0}, {0,0}, {-1,0}
    };

    const std::vector<std::complex<double>> SWAP = {
        {1,0}, {0,0}, {0,0}, {0,0},
        {0,0}, {0,0}, {1,0}, {0,0},
        {0,0}, {1,0}, {0,0}, {0,0},
        {0,0}, {0,0}, {0,0}, {1,0}
    };

} // namespace Gates

// Forward declarations for gate application functions
class StateVector;
void apply_single_qubit(StateVector& sv, uint8_t qubit, const Matrix2cd& gate);
void apply_cnot(StateVector& sv, uint8_t control, uint8_t target);
void apply_cz(StateVector& sv, uint8_t control, uint8_t target);
void apply_swap(StateVector& sv, uint8_t qubit1, uint8_t qubit2);
