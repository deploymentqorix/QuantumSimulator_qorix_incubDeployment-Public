# --- Project Configuration ---
# QCSIM_SRS_001: The project shall utilize CMake version 3.15 or newer.
cmake_minimum_required(VERSION 3.15)
project(QuantumSimulator VERSION 1.0 LANGUAGES CXX)

# --- Set C++ Standard and Compiler Features ---
# QCSIM_SRS_007: The project shall be compiled using the C++20 standard.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- ADD THIS BLOCK TO ENABLE AVX2 & FMA ---
# QCSIM_SRS_125: The simulator shall leverage AVX2 and FMA instructions for performance-critical matrix operations.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mfma")
endif()

# --- Find Dependencies ---
# QCSIM_SRS_006: The build system shall automatically detect and link the OpenMP library.
find_package(OpenMP REQUIRED)
find_package(Threads REQUIRED)

# --- Create the Core Simulator Library ---
# QCSIM_SRS_002: A CMakeLists.txt file shall manage all build targets, including the main library.
add_library(qsim_core
    # QCSIM_SRS_105: Implements the StateVector backend.
    src/StateVector.cpp
    src/StateVectorBackend.cpp
    # QCSIM_SRS_103: Defines standard quantum gates.
    src/Gates.cpp
    # QCSIM_SRS_101: Core simulation logic.
    src/Simulator.cpp
    src/QuantumCircuit.cpp
    # QCSIM_SRS_114: Implements the QASM parser.
    src/QASM_Parser.cpp
    # QCSIM_SRS_210: Contains benchmarking logic.
    src/Benchmark.cpp
    # QCSIM_SRS_110: Implements the DensityMatrix backend.
    src/DensityMatrix.cpp
    src/DensityMatrixBackend.cpp
    # QCSIM_SRS_111: Implements various noise channels.
    src/BitFlipChannel.cpp
    src/AmplitudeDampingChannel.cpp
    src/PhaseDampingChannel.cpp
    src/PhaseFlipChannel.cpp
    src/DepolarizingChannel.cpp
    # QCSIM_SRS_125: Contains optimized matrix utilities.
    src/MatrixUtils.cpp
    # Implements gate fusion optimization.
    src/GateFuser.cpp
)
target_include_directories(qsim_core PUBLIC
    include
    include/external
)

# --- Create the Command-Line Executable (CLI) ---
# QCSIM_SRS_201: The simulator shall provide a command-line interface.
add_executable(qsim_cli src/main_cli.cpp)
target_link_libraries(qsim_cli PRIVATE qsim_core OpenMP::OpenMP_CXX)

# --- Create the Web Server Executable ---
# QCSIM_SRS_302: A C++ backend server shall be implemented to handle simulation requests.
add_executable(qsim_server src/main_server.cpp)
target_link_libraries(qsim_server PRIVATE qsim_core OpenMP::OpenMP_CXX Threads::Threads)
enable_testing()

# Find GoogleTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Collect all test files
file(GLOB TEST_SOURCES tests/*.cpp)

# Create test executable
add_executable(runTests ${TEST_SOURCES})
target_link_libraries(runTests ${GTEST_LIBRARIES} pthread)

# Register tests
add_test(NAME QuantumSimulatorTests COMMAND runTests)
