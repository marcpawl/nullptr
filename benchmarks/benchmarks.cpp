#include <benchmark/benchmark.h>

// NOLINTBEGIN
// cppcheck-suppress constParameterCallback
static void BM_SomeFunction(benchmark::State& state) {
    for (auto _ : state) {
        // Your code to benchmark here
    }
}
BENCHMARK(BM_SomeFunction);

BENCHMARK_MAIN();
// NOLINTEND
