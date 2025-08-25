// ghz_test.qasm
// Creates a 3-qubit Greenberger-Horne-Zeilinger (GHZ) state.
// Expected outcome: 50% |000> and 50% |111>.

OPENQASM 2.0;
include "qelib1.inc";

qreg q[3];
creg c[3];

// Create the GHZ state
h q[0];
cx q[0], q[1];
cx q[0], q[2];

// Measure all qubits
measure q[0] -> c[0];
measure q[1] -> c[1];
measure q[2] -> c[2];
