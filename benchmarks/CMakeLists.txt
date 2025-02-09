cmake_minimum_required(VERSION 3.10)

# Find the Google Benchmark package
find_package(benchmark REQUIRED)

# Add your executable
add_executable(benchmarks benchmarks.cpp)

# Link Google Benchmark to your executable
target_link_libraries(benchmarks PRIVATE benchmark::benchmark)

