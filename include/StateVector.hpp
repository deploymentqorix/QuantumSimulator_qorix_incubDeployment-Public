#pragma once

#include <iostream>
#include <vector>
#include <complex>
#include <cstdint>
#include <stdexcept>
#include <cmath>

class StateVector {
public:
    explicit StateVector(uint8_t num_qubits);

    StateVector(const StateVector&) = delete;
    StateVector& operator=(const StateVector&) = delete;
    StateVector(StateVector&&) = default;
    StateVector& operator=(StateVector&&) = default;

    [[nodiscard]] uint8_t get_num_qubits() const;
    [[nodiscard]] const std::vector<std::complex<double>>& get_state_vector() const;
    
    // ADD THIS LINE to allow modification of the state vector
    [[nodiscard]] std::vector<std::complex<double>>& get_state_vector_mut();

    void display(std::ostream& os = std::cout) const;

private:
    uint8_t m_num_qubits;
    std::vector<std::complex<double>> m_state_vector;
};
