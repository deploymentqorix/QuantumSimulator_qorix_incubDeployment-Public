#pragma once

#include "DensityMatrix.hpp" // We need the DensityMatrix definition
#include <vector>

/**
 * @class QuantumChannel
 * @brief Abstract base class for all quantum noise channels.
 *
 * Defines the interface for applying a noise operation to a density matrix
 * using the operator-sum representation.
 *
 * QCSIM_SRS_149: An abstract base class QuantumChannel shall be defined.
 */
class QuantumChannel {
public:
    virtual ~QuantumChannel() = default;

    /**
     * @brief Applies the noise channel to a density matrix on a target qubit.
     * @param dm The DensityMatrix to modify.
     * @param target_qubit The qubit to apply the noise to.
     *
     * QCSIM_SRS_150: A QuantumChannel shall be applied using the operator-sum representation.
     */
    virtual void apply(DensityMatrix& dm, size_t target_qubit) const = 0;
};
