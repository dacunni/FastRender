cmake_minimum_required(VERSION 3.1)

PROJECT (fastrender_benchmarks)

ADD_EXECUTABLE (bench_vector    vector.cpp)
ADD_EXECUTABLE (bench_transform transform.cpp)
ADD_EXECUTABLE (bench_intersect intersect.cpp)
ADD_EXECUTABLE (bench_random    random.cpp)

set(BENCH_LIBS benchmark)

TARGET_LINK_LIBRARIES (bench_vector    FastRender ${LIBS} ${BENCH_LIBS})
TARGET_LINK_LIBRARIES (bench_transform FastRender ${LIBS} ${BENCH_LIBS})
TARGET_LINK_LIBRARIES (bench_intersect FastRender ${LIBS} ${BENCH_LIBS})
TARGET_LINK_LIBRARIES (bench_random    FastRender ${LIBS} ${BENCH_LIBS})

