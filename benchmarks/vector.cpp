#include <benchmark/benchmark.h>
#include "FastRender.h"

static void Vector4Creation(benchmark::State& state) {
    for (auto _ : state) {
        Vector4 r;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(Vector4Creation);

static void Vector4Add(benchmark::State& state) {
    Vector4 a, b, r;
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for (auto _ : state) {
        r = add(a, b);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(Vector4Add);

static void Vector4Dot(benchmark::State& state) {
    Vector4 a, b;
    float r;
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for (auto _ : state) {
        r = dot(a, b);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(Vector4Dot);

static void Vector4Cross(benchmark::State& state) {
    Vector4 a, b, r;
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for (auto _ : state) {
        r = cross(a, b);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(Vector4Cross);

BENCHMARK_MAIN();

