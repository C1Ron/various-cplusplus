#include "benchmark/benchmark.h"
#include "DateClass.h"    // Original DateClass
#include "DateClass2.h"   // Optimized DateClass

static void BM_Constructor1(benchmark::State& state) 
{
    for (auto _ : state) {
        DateClass date("monday", "jan", 2021);
        benchmark::DoNotOptimize(date);
    }
}
static void BM_Constructor2(benchmark::State& state) 
{
    for (auto _ : state) {
        DateClass2 date("monday", "jan", 2021);
        benchmark::DoNotOptimize(date);
    }
}
static void BM_SetDay1(benchmark::State& state) 
{
    DateClass date;
    for (auto _ : state) {
        date.setDay("wednesday");
        benchmark::DoNotOptimize(date);
    }
}
static void BM_SetDay2(benchmark::State& state) 
{
    DateClass2 date;
    for (auto _ : state) {
        date.setDay("wednesday");
        benchmark::DoNotOptimize(date);
    }
}
static void BM_SetMonth1(benchmark::State& state) 
{
    DateClass date;
    for (auto _ : state) {
        date.setMonth("jul");
        benchmark::DoNotOptimize(date);
    }
}
static void BM_SetMonth2(benchmark::State& state) 
{
    DateClass2 date;
    for (auto _ : state) {
        date.setMonth("jul");
        benchmark::DoNotOptimize(date);
    }
}
static void BM_PrintDate1(benchmark::State& state) 
{
    DateClass date("friday", "dec", 2023);
    for (auto _ : state) {
        std::string result = date.printDateToString();
        benchmark::DoNotOptimize(result);
    }
}
static void BM_PrintDate2(benchmark::State& state) 
{
    DateClass2 date("friday", "dec", 2023);
    for (auto _ : state) {
        std::string result = date.printDateToString();
        benchmark::DoNotOptimize(result);
    }
}








// Register the benchmarks
BENCHMARK(BM_Constructor1);
BENCHMARK(BM_Constructor2);
BENCHMARK(BM_SetDay1);
BENCHMARK(BM_SetDay2);
BENCHMARK(BM_SetMonth1);
BENCHMARK(BM_SetMonth2);
BENCHMARK(BM_PrintDate1);
BENCHMARK(BM_PrintDate2);

BENCHMARK_MAIN();