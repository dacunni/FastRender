#include <benchmark/benchmark.h>
#include "FastRender.h"

static void Vector4Creation(benchmark::State& state) {
    for (auto _ : state) {
        Vector4 r;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(Vector4Creation);

static void Vector4Magnitude(benchmark::State& state) {
    Vector4 v(1.3, 4.2, 5.6);
    for (auto _ : state) {
        benchmark::DoNotOptimize(v.magnitude());
    }
}
BENCHMARK(Vector4Magnitude);

static void Vector4MagnitudeSq(benchmark::State& state) {
    Vector4 v(1.3, 4.2, 5.6);
    for (auto _ : state) {
        benchmark::DoNotOptimize(v.magnitude_sq());
    }
}
BENCHMARK(Vector4MagnitudeSq);

static void Vector4Normalized(benchmark::State& state) {
    Vector4 v(1.3, 4.2, 5.6);
    for (auto _ : state) {
        benchmark::DoNotOptimize(v.normalized());
    }
}
BENCHMARK(Vector4Normalized);

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

static void Vector4Mirror(benchmark::State& state) {
    Vector4 a(0.2, 0.2, 0.3), n(0, 1, 0), r;
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(n);
    for (auto _ : state) {
        r = mirror(a, n);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(Vector4Mirror);

static void Vector4Refract(benchmark::State& state) {
    Vector4 a(0.2, 0.2, 0.3), n(0, 1, 0), r;
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(n);
    for (auto _ : state) {
        r = refract(a, n, 1.0, 1.3);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(Vector4Refract);

static void Matrix4x4Mult(benchmark::State& state) {
    Transform xf1 = makeRotation(0.3, Vector4(0.2, 0.3, 0.4));
    Transform xf2 = makeRotation(0.6, Vector4(0.9, 0.2, 0.1));
    for (auto _ : state) {
        benchmark::DoNotOptimize(mult(xf1.fwd, xf2.fwd));
    }
}
BENCHMARK(Matrix4x4Mult);

static void Matrix4x4Mult2(benchmark::State& state) {
    Transform xf1 = makeRotation(0.3, Vector4(0.2, 0.3, 0.4));
    Transform xf2 = makeRotation(0.6, Vector4(0.9, 0.2, 0.1));
    Matrix4x4 r;
    for (auto _ : state) {
        mult(xf1.fwd, xf2.fwd, r);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(Matrix4x4Mult2);

static void Matrix4x4Vector4Mult(benchmark::State& state) {
    Transform xf = makeRotation(0.3, Vector4(0.2, 0.3, 0.4));
    Vector4 v(2.0, 4.0, 0.3);
    for (auto _ : state) {
        benchmark::DoNotOptimize(mult(xf.fwd, v));
    }
}
BENCHMARK(Matrix4x4Vector4Mult);

static void Matrix4x4Vector4Mult2(benchmark::State& state) {
    Transform xf = makeRotation(0.3, Vector4(0.2, 0.3, 0.4));
    Vector4 v(2.0, 4.0, 0.3), r;
    for (auto _ : state) {
        mult(xf.fwd, v, r);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(Matrix4x4Vector4Mult2);

BENCHMARK_MAIN();

