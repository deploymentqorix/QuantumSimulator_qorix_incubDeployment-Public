#pragma once

#include "QuantumChannel.hpp"
#include <stdexcept>

/**
 * @class AmplitudeDampingChannel
 * @brief Implements an amplitude damping (T1) noise channel.
 *
 * Simulates energy loss from the environment.
 *
 * QCSIM_SRS_157: An AmplitudeDampingChannel class shall be implemented.
 */
class AmplitudeDampingChannel : public QuantumChannel {
public:
    /**
     * @brief Constructs an AmplitudeDampingChannel.
     * @param gamma The probability of a qubit losing a quantum of energy.
     *
     * QCSIM_SRS_158: The AmplitudeDampingChannel shall simulate energy loss with a parameter 'gamma'.
     */
    explicit AmplitudeDampingChannel(double gamma) : probability(gamma) {
        if (gamma < 0.0 || gamma > 1.0) {
            throw std::invalid_argument("Gamma (probability) must be between 0.0 and 1.0.");
        }
    }

    void apply(DensityMatrix& dm, size_t target_qubit) const override;

private:
    double probability;
};
