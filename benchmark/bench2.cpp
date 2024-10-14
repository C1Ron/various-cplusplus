#include "benchmark/benchmark.h"
#include "DateClass2.h"

static void BM_PrintDateToString1(benchmark::State& state) 
{
    DateClass2 date("friday", "dec", 2023);
    for (auto _ : state) {
        std::string result = date.printDateToString1();
        benchmark::DoNotOptimize(result);
    }
}

static void BM_PrintDateToString2(benchmark::State& state) 
{
    DateClass2 date("friday", "dec", 2023);
    for (auto _ : state) {
        std::string result = date.printDateToString2();
        benchmark::DoNotOptimize(result);
    }
}

static void BM_PrintDateToString3(benchmark::State& state) 
{
    DateClass2 date("friday", "dec", 2023);
    for (auto _ : state) {
        std::string result = date.printDateToString3();
        benchmark::DoNotOptimize(result);
    }
}

static void BM_PrintDateToString4(benchmark::State& state) 
{
    DateClass2 date("friday", "dec", 2023);
    for (auto _ : state) {
        std::string result = date.printDateToString4();
        benchmark::DoNotOptimize(result);
    }
}

BENCHMARK(BM_PrintDateToString1);
BENCHMARK(BM_PrintDateToString2);
BENCHMARK(BM_PrintDateToString3);
BENCHMARK(BM_PrintDateToString4);

BENCHMARK_MAIN();