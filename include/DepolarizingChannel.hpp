#pragma once

#include "QuantumChannel.hpp"
#include <stdexcept>

/**
 * @class DepolarizingChannel
 * @brief Implements a single-qubit depolarizing channel.
 *
 * With probability p, the state is randomized. With probability 1-p, it is unchanged.
 *
 * QCSIM_SRS_155: A DepolarizingChannel class shall be implemented.
 */
class DepolarizingChannel : public QuantumChannel {
public:
    /**
     * @brief Constructs a DepolarizingChannel.
     * @param p The probability of depolarization.
     *
     * QCSIM_SRS_056: The DepolarizingChannel shall take a single probability parameter 'p'.
     */
    explicit DepolarizingChannel(double p) : probability(p) {
        if (p < 0.0 || p > 1.0) {
            throw std::invalid_argument("Probability must be between 0.0 and 1.0.");
        }
    }

    void apply(DensityMatrix& dm, size_t target_qubit) const override;

private:
    double probability;
};