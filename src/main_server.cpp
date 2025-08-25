#include <iostream>
#include <memory>
#include <chrono>
#include <string>
#include "../include/httplib.h"
#include "../include/external/json.hpp"
#include "../include/Simulator.hpp"
#include "../include/QASM_Parser.hpp"
#include "../include/QuantumCircuit.hpp"
#include "../include/StateVectorBackend.hpp"
#include "../include/GateFuser.hpp"

// for convenience
using json = nlohmann::json;

// QCSIM_SRS_302: A C++ backend server shall be implemented to handle simulation requests.
int main() {
    httplib::Server svr;

    // --- Handler for CORS preflight requests ---
    svr.Options("/simulate", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.set_header("Access-Control-Allow-Methods", "POST, OPTIONS");
        res.status = 204;
    });

    // QCSIM_SRS_003: The server shall expose a RESTful API endpoint (e.g., /simulate) to receive QASM code.
    svr.Post("/simulate", [](const httplib::Request& req, httplib::Response& res) {
        // Allow requests from any origin for the actual POST request
        res.set_header("Access-Control-Allow-Origin", "*");
        
        json response_json;
        try {
            json request_json = json::parse(req.body);
            std::string qasm_content = request_json.at("qasm");
            bool fuse_gates = request_json.value("fuse_gates", false);
            int shots = request_json.value("shots", 1024);

            QASM_Parser parser;
            QuantumCircuit circuit = parser.parse(qasm_content);

            if (fuse_gates) {
                circuit = GateFuser::optimize(circuit);
            }

            auto backend = std::make_unique<StateVectorBackend>(circuit.get_num_qubits());
            Simulator sim(std::move(backend));

            auto start_time = std::chrono::high_resolution_clock::now();
            std::map<std::string, int> counts = sim.execute(circuit, shots);
            auto end_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration_ms = end_time - start_time;

            // QCSIM_SRS_005: The server shall return simulation results in JSON format.
            response_json["counts"] = counts;
            response_json["benchmark"] = {
                {"time_ms", duration_ms.count()},
                {"qubits", static_cast<int>(circuit.get_num_qubits())},
                {"gates_executed", circuit.get_ops().size()}
            };
            
            res.set_content(response_json.dump(2), "application/json");

        // QCSIM_SRS_006: The server shall handle potential errors during parsing or simulation and return a meaningful JSON error message.
        } catch (const json::parse_error& e) {
            res.status = 400;
            response_json["message"] = "Invalid JSON format: " + std::string(e.what());
            res.set_content(response_json.dump(2), "application/json");
        } catch (const std::exception& e) {
            res.status = 500;
            response_json["message"] = "Simulation error: " + std::string(e.what());
            res.set_content(response_json.dump(2), "application/json");
        }
    });

    std::cout << "Server starting on http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}
