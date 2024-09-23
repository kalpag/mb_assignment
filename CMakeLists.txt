# Minimum required version of CMake
cmake_minimum_required(VERSION 3.14)

# Project name and C++ standard
project(mollybet LANGUAGES CXX)

# Use C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include the directory for header files
include_directories(include)
include_directories(cpr)

add_subdirectory(cpr)

# Find and link necessary libraries (Boost, OpenSSL, etc.)
find_package(Boost REQUIRED COMPONENTS system thread)
find_package(OpenSSL REQUIRED)

# Add the executable target and link the source files
add_executable(molly_api_client
    src/main.cpp
    src/HTTPClient.cpp
    src/WebSocketClient.cpp
    src/MollyAPIClient.cpp
)

# Link libraries to the target executable
target_link_libraries(molly_api_client
    Boost::system
    Boost::thread
    OpenSSL::SSL
    cpr::cpr
)

# Specify where the target binary should be placed after build
set_target_properties(molly_api_client PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
