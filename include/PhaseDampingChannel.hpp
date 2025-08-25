#pragma once

#include "QuantumChannel.hpp"
#include <stdexcept>

/**
 * @class PhaseDampingChannel
 * @brief Implements a phase damping (T2) noise channel.
 *
 * Simulates the loss of quantum coherence without loss of energy.
 *
 * QCSIM_SRS_159: A PhaseDampingChannel class shall be implemented.
 */
class PhaseDampingChannel : public QuantumChannel {
public:
    /**
     * @brief Constructs a PhaseDampingChannel.
     * @param gamma The probability of a phase damping error occurring.
     *
     * QCSIM_SRS_160: The PhaseDampingChannel shall simulate loss of quantum
     * information without energy loss with a parameter 'gamma'.
     */
    explicit PhaseDampingChannel(double gamma) : probability(gamma) {
        if (gamma < 0.0 || gamma > 1.0) {
            throw std::invalid_argument("Gamma (probability) must be between 0.0 and 1.0.");
        }
    }

    void apply(DensityMatrix& dm, size_t target_qubit) const override;

private:
    double probability;
};
