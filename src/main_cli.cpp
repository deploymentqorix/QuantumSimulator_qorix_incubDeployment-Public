#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <memory>
#include <vector>
#include <map>
#include <chrono>

#include "CLI11.hpp"
#include "QASM_Parser.hpp"
#include "Simulator.hpp"
#include "QuantumCircuit.hpp"
#include "StateVectorBackend.hpp"
#include "DensityMatrixBackend.hpp"
#include "BitFlipChannel.hpp"
#include "AmplitudeDampingChannel.hpp"
#include "PhaseDampingChannel.hpp"
#include "PhaseFlipChannel.hpp"
#include "DepolarizingChannel.hpp"
#include "GateFuser.hpp"

// --- Helper functions for formatting output ---
// QCSIM_SRS_204: The JSON output shall contain a dictionary of measurement bitstrings and their counts.
std::string format_as_json(const std::map<std::string, int>& counts) {
    std::string json = "{\n";
    for (auto it = counts.begin(); it != counts.end(); ++it) {
        json += "  \"" + it->first + "\": " + std::to_string(it->second);
        if (std::next(it) != counts.end()) {
            json += ",";
        }
        json += "\n";
    }
    json += "}";
    return json;
}

// QCSIM_SRS_205: The CSV output shall contain two columns: 'outcome' and 'count'.
std::string format_as_csv(const std::map<std::string, int>& counts) {
    std::string csv = "outcome,count\n";
    for (const auto& [outcome, count] : counts) {
        csv += outcome + "," + std::to_string(count) + "\n";
    }
    return csv;
}

// QCSIM_SRS_201: The simulator shall provide a command-line interface.
int main(int argc, char** argv) {
    CLI::App app{"Quantum Simulator CLI"};
    app.set_version_flag("--version,-v", "QuantumSimulator 1.0");

    // QCSIM_SRS_202: The CLI shall accept a mandatory argument for the path to the QASM file.
    std::string qasm_file_path;
    app.add_option("qasm_file", qasm_file_path, "Path to the OpenQASM 2.0 file")
        ->required()->check(CLI::ExistingFile);

    // QCSIM_SRS_129: The simulator shall support a configurable number of shots for measurement statistics.
    int shots = 1024;
    app.add_option("--shots,-s", shots, "Number of times to run the circuit");

    // QCSIM_SRS_206: The CLI shall provide a flag to select the simulation backend.
    std::string backend_name = "statevector";
    app.add_option("--backend,-b", backend_name, "Simulation backend ('statevector' or 'density_matrix')")
        ->check(CLI::IsMember({"statevector", "density_matrix"}));

    // QCSIM_SRS_207: A flag shall be available to print the final quantum state.
    bool print_state_flag = false;
    app.add_flag("--print-state", print_state_flag, "Print the final quantum state to the console.");

    // QCSIM_SRS_203: The CLI shall allow saving results to a specified output file.
    std::string output_path;
    app.add_option("--output,-o", output_path, "Path to save the output results file.");

    std::string format = "json";
    app.add_option("--format", format, "Output file format ('json' or 'csv')")
        ->check(CLI::IsMember({"json", "csv"}));

    // QCSIM_SRS_210: A benchmark mode shall be available via a CLI flag.
    bool benchmark_mode = false;
    app.add_flag("--benchmark", benchmark_mode, "Run in benchmark mode to measure performance.");

    bool fuse_gates = false;
    app.add_flag("--fuse-gates", fuse_gates, "Enable gate fusion optimization.");


    // QCSIM_SRS_111: The simulator shall support various noise channels, configurable via the CLI.
    double bit_flip_prob = 0.0, amp_damp_gamma = 0.0, phase_damp_gamma = 0.0, phase_flip_prob = 0.0, depolarizing_prob = 0.0;
    app.add_option("--noise-bitflip", bit_flip_prob, "Apply bit-flip noise.");
    app.add_option("--noise-amplitude-damping", amp_damp_gamma, "Apply T1 amplitude damping noise.");
    app.add_option("--noise-phase-damping", phase_damp_gamma, "Apply T2 phase damping noise.");
    app.add_option("--noise-phaseflip", phase_flip_prob, "Apply phase-flip (Z-error) noise.");
    app.add_option("--noise-depolarizing", depolarizing_prob, "Apply depolarizing noise.");
    auto noise_group = app.add_option_group("Noise Models", "Choose at most one noise model");
    noise_group->add_options(app.get_option("--noise-bitflip"), app.get_option("--noise-amplitude-damping"), app.get_option("--noise-phase-damping"), app.get_option("--noise-phaseflip"), app.get_option("--noise-depolarizing"));
    noise_group->require_option(0, 1);

    CLI11_PARSE(app, argc, argv);

    // QCSIM_SRS_208: The CLI shall handle errors gracefully and provide informative messages.
    try {
        std::ifstream file(qasm_file_path);
        std::string qasm_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        QASM_Parser parser;
        QuantumCircuit circuit = parser.parse(qasm_content);
        
        if (fuse_gates) {
            std::cout << "Applying gate fusion optimization..." << std::endl;
            circuit = GateFuser::optimize(circuit);
        }

        bool is_noisy = bit_flip_prob > 0.0 || amp_damp_gamma > 0.0 || phase_damp_gamma > 0.0 || phase_flip_prob > 0.0 || depolarizing_prob > 0.0;
        if (is_noisy) backend_name = "density_matrix";

        std::cout << "Successfully parsed QASM file: " << qasm_file_path << std::endl;
        std::cout << "Backend: " << backend_name << std::endl;
        
        std::shared_ptr<QuantumChannel> noise_channel = nullptr;
        if (bit_flip_prob > 0.0) noise_channel = std::make_shared<BitFlipChannel>(bit_flip_prob);
        else if (amp_damp_gamma > 0.0) noise_channel = std::make_shared<AmplitudeDampingChannel>(amp_damp_gamma);
        else if (phase_damp_gamma > 0.0) noise_channel = std::make_shared<PhaseDampingChannel>(phase_damp_gamma);
        else if (phase_flip_prob > 0.0) noise_channel = std::make_shared<PhaseFlipChannel>(phase_flip_prob);
        else if (depolarizing_prob > 0.0) noise_channel = std::make_shared<DepolarizingChannel>(depolarizing_prob);

        std::unique_ptr<SimulationBackend> backend;
        if (backend_name == "statevector") {
            backend = std::make_unique<StateVectorBackend>(circuit.get_num_qubits());
        } else {
            auto dm_backend = std::make_unique<DensityMatrixBackend>(circuit.get_num_qubits());
            if (is_noisy) dm_backend->set_noise_channel(noise_channel);
            backend = std::move(dm_backend);
        }

        Simulator sim(std::move(backend));

        auto start_time = std::chrono::high_resolution_clock::now();

        std::map<std::string, int> counts = sim.execute(circuit, shots);

        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration_ms = end_time - start_time;

        if (benchmark_mode) {
            std::cout << "\n--- Benchmark Results ---" << std::endl;
            std::cout << "Circuit File:    " << qasm_file_path << std::endl;
            std::cout << "Qubits:          " << static_cast<int>(circuit.get_num_qubits()) << std::endl;
            std::cout << "Gates:           " << circuit.get_ops().size() << std::endl;
            std::cout << "Shots:           " << shots << std::endl;
            std::cout << "Total Time (ms): " << duration_ms.count() << std::endl;
        } else {
            if (print_state_flag) {
                std::cout << "\n--- Final Quantum State ---" << std::endl;
                sim.print_final_state();
            }

            if (!output_path.empty()) {
                std::ofstream output_file(output_path);
                if (!output_file) {
                    throw std::runtime_error("Error: Could not open output file: " + output_path);
                }
                if (format == "json") {
                    output_file << format_as_json(counts);
                } else { // csv
                    output_file << format_as_csv(counts);
                }
                std::cout << "\nResults saved to " << output_path << std::endl;
            } else {
                std::cout << "\n--- Results ---" << std::endl;
                for (const auto& [outcome, count] : counts) {
                    std::cout << outcome << ": " << count << std::endl;
                }
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
