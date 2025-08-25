#pragma once

#include "QuantumChannel.hpp"
#include <stdexcept>

/**
 * @class PhaseFlipChannel
 * @brief Implements a phase-flip (Z-error) noise channel.
 *
 * QCSIM_SRS_153: A PhaseFlipChannel class shall be implemented.
 */
class PhaseFlipChannel : public QuantumChannel {
public:
    /**
     * @brief Constructs a PhaseFlipChannel.
     * @param p The probability of a phase-flip occurring.
     *
     * QCSIM_SRS_154: The PhaseFlipChannel shall take a single probability parameter 'p'.
     */
    explicit PhaseFlipChannel(double p) : probability(p) {
        if (p < 0.0 || p > 1.0) {
            throw std::invalid_argument("Probability must be between 0.0 and 1.0.");
        }
    }

    void apply(DensityMatrix& dm, size_t target_qubit) const override;

private:
    double probability;
};