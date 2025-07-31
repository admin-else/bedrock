cmake_minimum_required(VERSION 3.10)
project(bedvers C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find OpenSSL package (required for bedrock target)
find_package(OpenSSL REQUIRED)

# Bedrock executable
add_executable(bedrock bedrock.c)
target_link_libraries(bedrock PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# Enable optimizations for Release builds
target_compile_options(bedrock PRIVATE 
    $<$<CONFIG:Release>:-O3>)

# Drops executable
add_executable(drops drops.c)

target_link_libraries(drops PRIVATE OpenSSL::SSL OpenSSL::Crypto)
# Set output directories
target_compile_options(bedrock PRIVATE
    $<$<CONFIG:Release>:-O3>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)