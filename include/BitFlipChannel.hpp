#pragma once

#include "QuantumChannel.hpp"
#include <stdexcept>

/**
 * @class BitFlipChannel
 * @brief Implements a bit-flip noise channel.
 *
 * This channel applies an X gate with probability 'p' and the identity
 * with probability '1-p'.
 *
 * QCSIM_SRS_151: A BitFlipChannel class shall be implemented.
 */
class BitFlipChannel : public QuantumChannel {
public:
    /**
     * @brief Constructs a BitFlipChannel.
     * @param p The probability of a bit-flip occurring.
     *
     * QCSIM_SRS_152: The BitFlipChannel shall take a single probability parameter 'p'.
     */
    explicit BitFlipChannel(double p) : probability(p) {
        if (p < 0.0 || p > 1.0) {
            throw std::invalid_argument("Probability must be between 0.0 and 1.0.");
        }
    }

    // This method will be implemented in the corresponding .cpp file.
    void apply(DensityMatrix& dm, size_t target_qubit) const override;

private:
    double probability;
};
