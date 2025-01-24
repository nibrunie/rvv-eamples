// file: bench_matrix_transpose.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <bench_utils.h>
#include "ubench_utils.h"


/** Number of executed test(s) */
#ifndef NUM_TESTS
#define NUM_TESTS 10
#endif
#ifndef TEST_SIZE
#define TEST_SIZE 65536
#endif

/** Declaring various softmax implementation benchmarks **/
ubench_result_t baseline_bench(size_t n);


ubench_result_t baseline_bench(size_t n) {
    long start = read_perf_counter();
    for (int i = 0; i < n / 16; i++) {
        asm volatile(
            "nop\n"
            "nop\n"
            "nop\n"
            "nop\n"
            "nop\n"
            "nop\n"
            "nop\n"
            "nop\n"
            "nop\n"
            "nop\n"
            "nop\n"
            "nop\n"
            "nop\n"
            "nop\n"
            "nop\n"
            "nop\n"
            :
            :
            :
        );
        // do nothing
    }
    long stop = read_perf_counter();
    return (ubench_result_t){
        .perf_count = (stop - start),
        .elt_per_op = 1,
        .errors = 0
    };
};

BENCH_0OP_INSN(nop)
BENCH_LAT_2OP_INSN(add)
BENCH_LAT_2OP_INSN(div)
BENCH_LAT_2OP_INSN(divu)
BENCH_LAT_2OP_INSN(divw)
BENCH_LAT_2OP_INSN(divuw)
BENCH_LAT_2OP_INSN(rem)
BENCH_LAT_2OP_INSN(remu)
BENCH_LAT_2OP_INSN(remw)
BENCH_LAT_2OP_INSN(remuw)
BENCH_LAT_2OP_INSN(mul)
BENCH_LAT_2OP_INSN(mulh)
BENCH_LAT_2OP_INSN(mulhsu)
BENCH_LAT_2OP_INSN(mulhu)
BENCH_LAT_2OP_INSN(mulw)

BENCH_THROUGHPUT_2OP_INSN(add)
BENCH_THROUGHPUT_2OP_INSN(div)
BENCH_THROUGHPUT_2OP_INSN(divu)
BENCH_THROUGHPUT_2OP_INSN(divw)
BENCH_THROUGHPUT_2OP_INSN(divuw)
BENCH_THROUGHPUT_2OP_INSN(rem)
BENCH_THROUGHPUT_2OP_INSN(remu)
BENCH_THROUGHPUT_2OP_INSN(remw)
BENCH_THROUGHPUT_2OP_INSN(remuw)
BENCH_THROUGHPUT_2OP_INSN(mul)
BENCH_THROUGHPUT_2OP_INSN(mulh)
BENCH_THROUGHPUT_2OP_INSN(mulhsu)
BENCH_THROUGHPUT_2OP_INSN(mulhu)
BENCH_THROUGHPUT_2OP_INSN(mulw)


BENCH_LAT_2OP_FPD_INSN(fadd)
BENCH_LAT_2OP_FPD_INSN(fsub)
BENCH_LAT_2OP_FPD_INSN(fdiv)
BENCH_LAT_2OP_FPD_INSN(fmul)

BENCH_LAT_2OP_FPS_INSN(fadd)
BENCH_LAT_2OP_FPS_INSN(fsub)
BENCH_LAT_2OP_FPS_INSN(fdiv)
BENCH_LAT_2OP_FPS_INSN(fmul)

BENCH_THROUGHPUT_2OP_FPD_INSN(fadd)
BENCH_THROUGHPUT_2OP_FPD_INSN(fsub)
BENCH_THROUGHPUT_2OP_FPD_INSN(fdiv)
BENCH_THROUGHPUT_2OP_FPD_INSN(fmul)

BENCH_THROUGHPUT_2OP_FPS_INSN(fadd)
BENCH_THROUGHPUT_2OP_FPS_INSN(fsub)
BENCH_THROUGHPUT_2OP_FPS_INSN(fdiv)
BENCH_THROUGHPUT_2OP_FPS_INSN(fmul)


#define BENCH_FUNC_LAT_2OP_LMUL_1_4(op, eltSize, suffix) \
    BENCH_LAT_2OP_SUFFIX_VEC_INSN(op, 1, eltSize, suffix) \
    BENCH_LAT_2OP_SUFFIX_VEC_INSN(op, 2, eltSize, suffix) \
    BENCH_LAT_2OP_SUFFIX_VEC_INSN(op, 4, eltSize, suffix)

/** generate latency benchmark functions for a 2-operand .vs instruction
 *   spanning LMUL from 1 to 8, and setting SEW to eltSize
 */
#define BENCH_FUNC_LAT_2OP_LMUL_SPAN(op, eltSize, suffix) \
    BENCH_FUNC_LAT_2OP_LMUL_1_4(op, eltSize, suffix) \
    BENCH_LAT_2OP_SUFFIX_VEC_INSN(op, 8, eltSize, suffix)

/** generate throughput benchmark functions for a 2-operand .vs instruction
 *   spanning LMUL from 1 to 4, and setting SEW to eltSize
 */
#define BENCH_FUNC_THROUGHPUT_2OP_LMUL_1_4(op, eltSize, suffix) \
    BENCH_THROUGHPUT_2OP_SUFFIX_VEC_INSN(op, 1, eltSize, suffix) \
    BENCH_THROUGHPUT_2OP_SUFFIX_VEC_INSN(op, 2, eltSize, suffix) \
    BENCH_THROUGHPUT_2OP_SUFFIX_VEC_INSN(op, 4, eltSize, suffix)

#define BENCH_2OP_VV_VEC_INSN(op) \
    BENCH_FUNC_LAT_2OP_LMUL_SPAN(op, 16, vv) \
    BENCH_FUNC_LAT_2OP_LMUL_SPAN(op, 32, vv) \
    BENCH_FUNC_LAT_2OP_LMUL_SPAN(op, 64, vv) \
    BENCH_FUNC_THROUGHPUT_2OP_LMUL_1_4(op, 16, vv) \
    BENCH_FUNC_THROUGHPUT_2OP_LMUL_1_4(op, 32, vv) \
    BENCH_FUNC_THROUGHPUT_2OP_LMUL_1_4(op, 64, vv)

#define BENCH_2OP_VS_VEC_INSN(op) \
    BENCH_FUNC_LAT_2OP_LMUL_SPAN(op, 32, vs) \
    BENCH_FUNC_LAT_2OP_LMUL_SPAN(op, 64, vs) \
    BENCH_FUNC_THROUGHPUT_2OP_LMUL_1_4(op, 32, vs) \
    BENCH_FUNC_THROUGHPUT_2OP_LMUL_1_4(op, 64, vs) \
    // BENCH_FUNC_LAT_2OP_LMUL_SPAN(op, 16, vs) \
    // BENCH_FUNC_THROUGHPUT_2OP_LMUL_1_4(op, 16, vs) \

#define BENCH_2OP_WV_VEC_INSN(op) \
    BENCH_FUNC_LAT_2OP_LMUL_SPAN(op, 16, wv) \
    BENCH_FUNC_LAT_2OP_LMUL_SPAN(op, 32, wv) \
    BENCH_FUNC_THROUGHPUT_2OP_LMUL_1_4(op, 16, wv) \
    BENCH_FUNC_THROUGHPUT_2OP_LMUL_1_4(op, 32, wv)

#define BENCH_VEC_INSN_TC(op, suffix) \
        BENCH_LAT_VEC_INSN_TC(op, 1, 32, suffix),\
        BENCH_LAT_VEC_INSN_TC(op, 2, 32, suffix),\
        BENCH_LAT_VEC_INSN_TC(op, 4, 32, suffix),\
        BENCH_THROUGHPUT_VEC_INSN_TC(op, 1, 32, suffix),\
        BENCH_THROUGHPUT_VEC_INSN_TC(op, 2, 32, suffix),\
        // BENCH_LAT_VEC_INSN_TC(op, 1, 16, suffix),\
        // BENCH_LAT_VEC_INSN_TC(op, 2, 16, suffix),\
        // BENCH_LAT_VEC_INSN_TC(op, 4, 16, suffix),\
        // BENCH_THROUGHPUT_VEC_INSN_TC(op, 1, 16, suffix),\
        // BENCH_THROUGHPUT_VEC_INSN_TC(op, 2, 16, suffix),\

/* generating test case list */
#define BENCH_VEC_WV_INSN_TC(op)  BENCH_VEC_INSN_TC(op, wv)
#define BENCH_VEC_VV_INSN_TC(op)  BENCH_VEC_INSN_TC(op, vv)
#define BENCH_VEC_VS_INSN_TC(op)  BENCH_VEC_INSN_TC(op, vs)

/** generating benchmark function for a widening instruction
 *  with .vv suffix (cannot be evaluated on LMUL=8)
 */
#define BENCH_2OP_WVV_VEC_INSN(op) \
    BENCH_FUNC_LAT_2OP_LMUL_1_4(op, 16, vv) \
    BENCH_FUNC_LAT_2OP_LMUL_1_4(op, 32, vv) \
    BENCH_FUNC_THROUGHPUT_2OP_LMUL_1_4(op, 16, vv) \
    BENCH_FUNC_THROUGHPUT_2OP_LMUL_1_4(op, 32, vv)

BENCH_2OP_VV_VEC_INSN(vadd)
BENCH_2OP_VV_VEC_INSN(vor)
BENCH_2OP_VV_VEC_INSN(vxor)
BENCH_2OP_VV_VEC_INSN(vsll)
BENCH_2OP_WV_VEC_INSN(vnsrl)
BENCH_2OP_WV_VEC_INSN(vwadd)
BENCH_2OP_WV_VEC_INSN(vwsub)
BENCH_2OP_WVV_VEC_INSN(vwadd)
BENCH_2OP_WVV_VEC_INSN(vwmul)
BENCH_2OP_WVV_VEC_INSN(vfwadd)
BENCH_2OP_WVV_VEC_INSN(vfwmul)
BENCH_2OP_VV_VEC_INSN(vrgather)
BENCH_2OP_VS_VEC_INSN(vredsum)
BENCH_2OP_VS_VEC_INSN(vwredsum)
BENCH_2OP_VS_VEC_INSN(vredand)
BENCH_2OP_VS_VEC_INSN(vredor)
BENCH_2OP_VS_VEC_INSN(vredxor)
BENCH_2OP_VS_VEC_INSN(vfredosum)
BENCH_2OP_VS_VEC_INSN(vfredusum)
BENCH_2OP_VV_VEC_INSN(vfadd)
BENCH_2OP_VV_VEC_INSN(vfmul)
BENCH_2OP_VV_VEC_INSN(vfdiv)

/** generic data generator */
void my_data_gen(uint64_t res[]) {
    res[0] = 1;
    res[1] = 2;
}

void my_memcpy(void* dst, void* src, size_t nBytes) {

    __asm volatile (
        "mv t0, %[nBytes]\n" // avl
        "1:\n"
        "vsetvli t1, t0, e8, m1, ta, ma\n"
        "vle8.v v8, (%[src])\n"
        "vse8.v v8, (%[dst])\n"
        "add %[src], %[src], t1\n"
        "add %[dst], %[dst], t1\n"
        "sub t0, t0, t1\n"
        "bnez t0, 1b\n"
        : [src]"+r"(src), [dst]"+r"(dst)
        : [nBytes]"r"(nBytes)
        : "t0"
    );
}

int main(void) {
    int i;
    ubench_t benchmarks[] = {
        (ubench_t){.bench = baseline_bench,  .label="baseline benchmark"},
        BENCH_LAT_INSN_TC(nop),
        BENCH_LAT_INSN_TC(add),
        BENCH_LAT_INSN_TC(div),
        BENCH_LAT_INSN_TC(divu),
        BENCH_LAT_INSN_TC(divw),
        BENCH_LAT_INSN_TC(divuw),
        BENCH_LAT_INSN_TC(rem),
        BENCH_LAT_INSN_TC(remu),
        BENCH_LAT_INSN_TC(remw),
        BENCH_LAT_INSN_TC(remuw),
        BENCH_LAT_INSN_TC(mul),
        BENCH_LAT_INSN_TC(mulh),
        BENCH_LAT_INSN_TC(mulhsu),
        BENCH_LAT_INSN_TC(mulhu),
        BENCH_LAT_INSN_TC(mulw),

        BENCH_LAT_INSN_TC(fadd_d),
        BENCH_LAT_INSN_TC(fsub_d),
        BENCH_LAT_INSN_TC(fdiv_d),
        BENCH_LAT_INSN_TC(fmul_d),

        BENCH_LAT_INSN_TC(fadd_s),
        BENCH_LAT_INSN_TC(fsub_s),
        BENCH_LAT_INSN_TC(fdiv_s),
        BENCH_LAT_INSN_TC(fmul_s),

        BENCH_VEC_VV_INSN_TC(vadd)
        BENCH_VEC_VV_INSN_TC(vor)
        BENCH_VEC_VV_INSN_TC(vxor)
        BENCH_VEC_VV_INSN_TC(vsll)
        BENCH_VEC_VV_INSN_TC(vfadd)
        BENCH_VEC_VV_INSN_TC(vfmul)
        BENCH_VEC_VV_INSN_TC(vfdiv)
        BENCH_VEC_VV_INSN_TC(vrgather)

        BENCH_VEC_WV_INSN_TC(vnsrl)

        BENCH_VEC_WV_INSN_TC(vwadd)
        BENCH_VEC_WV_INSN_TC(vwsub)

        BENCH_VEC_VV_INSN_TC(vfwadd)
        BENCH_VEC_VV_INSN_TC(vfwmul)

        BENCH_VEC_VV_INSN_TC(vwmul)

        BENCH_VEC_VS_INSN_TC(vredsum)
        BENCH_VEC_VS_INSN_TC(vwredsum)
        BENCH_VEC_VS_INSN_TC(vredxor)
        BENCH_VEC_VS_INSN_TC(vredor)
        BENCH_VEC_VS_INSN_TC(vredand)
        BENCH_VEC_VS_INSN_TC(vfredosum)
        BENCH_VEC_VS_INSN_TC(vfredusum)

        BENCH_THROUGHPUT_INSN_TC(add),
        BENCH_THROUGHPUT_INSN_TC(div),
        BENCH_THROUGHPUT_INSN_TC(divu),
        BENCH_THROUGHPUT_INSN_TC(divw),
        BENCH_THROUGHPUT_INSN_TC(divuw),
        BENCH_THROUGHPUT_INSN_TC(rem),
        BENCH_THROUGHPUT_INSN_TC(remu),
        BENCH_THROUGHPUT_INSN_TC(remw),
        BENCH_THROUGHPUT_INSN_TC(remuw),
        BENCH_THROUGHPUT_INSN_TC(mul),
        BENCH_THROUGHPUT_INSN_TC(mulh),
        BENCH_THROUGHPUT_INSN_TC(mulhsu),
        BENCH_THROUGHPUT_INSN_TC(mulhu),
        BENCH_THROUGHPUT_INSN_TC(mulw),

        BENCH_THROUGHPUT_INSN_TC(fadd_d),
        BENCH_THROUGHPUT_INSN_TC(fsub_d),
        BENCH_THROUGHPUT_INSN_TC(fdiv_d),
        BENCH_THROUGHPUT_INSN_TC(fmul_d),

        BENCH_THROUGHPUT_INSN_TC(fadd_s),
        BENCH_THROUGHPUT_INSN_TC(fsub_s),
        BENCH_THROUGHPUT_INSN_TC(fdiv_s),
        BENCH_THROUGHPUT_INSN_TC(fmul_s),
    };

    // list of buffer sizes for memory copy throughput benchmark
    size_t memCopySizes[] = {1024, 4096, 16384, 32768, 49152, 65536}; //, (3ull << 15), (1ull << 17), (1ull << 20), (1ull << 24), (1ull << 27)};

#ifndef VERBOSE
    // condensed display
    printf("label, test-size, counter-value, metric-per-element, element-per-metric, error(s)\n");
#endif

    size_t testSizes[] = {TEST_SIZE};
    for (size_t testId = 0; testId < sizeof(testSizes) / sizeof(size_t); testId++)
    {
        size_t n = testSizes[testId];
#       ifdef VERBOSE 
        printf("--------------------------------------------------------------------------------\n");
        printf("--------------------------------------------------------------------------------\n");
        printf("Benchmarking with test size %d.\n", n);
#       endif

        // reset benchmark results
        for (unsigned benchId=0; benchId < sizeof(benchmarks) / sizeof(ubench_t); benchId++)
        {
            benchmarks[benchId].result.errors = 0;
            benchmarks[benchId].result.perf_count = 0;
        }

        for (int j = 0; j < NUM_TESTS; ++j) {
            for (unsigned benchId=0; benchId < sizeof(benchmarks) / sizeof(ubench_t); benchId++)
            {
#               ifdef VERY_VERBOSE
                printf("running method: %s\n", benchmarks[benchId].label);
#               endif // VERY_VERBOSE

                ubench_result_t local_result = benchmarks[benchId].bench(n);
                benchmarks[benchId].result = accumulate_ubench_result(benchmarks[benchId].result, local_result);
            }
        }

        // display results
        for (unsigned benchId=0; benchId < sizeof(benchmarks) / sizeof(ubench_t); benchId++)
        {
            ubench_result_t bench_result = benchmarks[benchId].result;
            bench_result.perf_count = bench_result.perf_count / NUM_TESTS;


#           ifdef VERBOSE 
            printf("--------------------------------------------------------------------------------\n");
            printf("%s used %d " PERF_METRIC "(s) to evaluate multiplication on a degree %d polynomial.\n",
                benchmarks[benchId].label, bench_result.perf_count, n);
            printf("  " PERF_METRIC " per run:        %d\n", bench_result.perf_count);
            printf("  " PERF_METRIC " per element:    %.3f\n", (double) bench_result.perf_count / (n * bench_result.elt_per_op));
            printf("  element(s) per " PERF_METRIC ": %.2e\n", (double) (n * bench_result.elt_per_op) / bench_result.perf_count);
            printf("  error(s):  %d\n", bench_result.errors);
#           else
            // condensed display
            printf("%s, %d, %d, %.3f, %.2f, %d\n", 
                   benchmarks[benchId].label, n, bench_result.perf_count,
                   (double) bench_result.perf_count / (n * bench_result.elt_per_op), (double) (n * bench_result.elt_per_op) / bench_result.perf_count,
                   bench_result.errors);
#           endif
        }
    }


    // data dependent benchmarking
    ubench_data_t data_benchmarks[] = {
        (ubench_data_t){.bench = bench_throughput_div_values, .data_gen=my_data_gen, .label="data division benchmark"},
    };
    for (size_t testId = 0; testId < sizeof(testSizes) / sizeof(size_t); testId++)
    {
        size_t n = testSizes[testId];
#       ifdef VERBOSE 
        printf("--------------------------------------------------------------------------------\n");
        printf("--------------------------------------------------------------------------------\n");
        printf("Data Benchmarking with test size %d.\n", n);
#       endif

        // reset benchmark results
        for (unsigned benchId=0; benchId < sizeof(data_benchmarks) / sizeof(ubench_data_t); benchId++)
        {
            data_benchmarks[benchId].result.errors = 0;
            data_benchmarks[benchId].result.perf_count = 0;
        }

        for (int j = 0; j < NUM_TESTS; ++j) {
            for (unsigned benchId=0; benchId < sizeof(data_benchmarks) / sizeof(ubench_data_t); benchId++)
            {
#               ifdef VERY_VERBOSE
                printf("running method: %s\n", benchmarks[benchId].label);
#               endif // VERY_VERBOSE

                uint64_t data[2] = {0};
                data_benchmarks[benchId].data_gen(data);
                ubench_result_t local_result = data_benchmarks[benchId].bench(n, data);
                data_benchmarks[benchId].result = accumulate_ubench_result(data_benchmarks[benchId].result, local_result);
            }
        }
        // display results
        for (unsigned benchId=0; benchId < sizeof(data_benchmarks) / sizeof(ubench_data_t); benchId++)
        {
            ubench_result_t bench_result = data_benchmarks[benchId].result;
            bench_result.perf_count = bench_result.perf_count / NUM_TESTS;


#           ifdef VERBOSE 
            printf("--------------------------------------------------------------------------------\n");
            printf("%s used %d " PERF_METRIC "(s) to evaluate multiplication on a degree %d polynomial.\n",
                data_benchmarks[benchId].label, bench_result.perf_count, n);
            printf("  " PERF_METRIC " per run:        %d\n", bench_result.perf_count);
            printf("  " PERF_METRIC " per element:    %.3f\n", (double) bench_result.perf_count / (n * bench_result.elt_per_op));
            printf("  element(s) per " PERF_METRIC ": %.2e\n", (double) (n * bench_result.elt_per_op) / bench_result.perf_count);
            printf("  error(s):  %d\n", bench_result.errors);
#           else
            // condensed display
            printf("%s, %d, %d, %.3f, %.2f, %d\n", 
                   data_benchmarks[benchId].label, n, bench_result.perf_count,
                   (double) bench_result.perf_count / (n * bench_result.elt_per_op), (double) (n * bench_result.elt_per_op) / bench_result.perf_count,
                   bench_result.errors);
#           endif
        }
    }

    // memory copy benchmarks
    size_t nRuns = 20;

    size_t maxSize = 0;
    for (size_t testId = 0; testId < sizeof(memCopySizes) / sizeof(size_t); testId++) maxSize = maxSize < memCopySizes[testId] ? memCopySizes[testId] : maxSize;

    char* memSrc = (char*) malloc(maxSize);
    char* memDst = (char*) malloc(maxSize);
    for (int i = 0; i < maxSize; i++) memSrc[i] = rand();

    for (size_t testId = 0; testId < sizeof(memCopySizes) / sizeof(size_t); testId++) {
        char* dst = memDst;
        char* src = memSrc;
        char* swap = NULL;
        size_t localSize = memCopySizes[testId];
        long start = read_perf_counter();
        for (int runId = 0; runId < nRuns; runId++) {
            my_memcpy(dst, src, localSize);

            // swapping source/destination address for the next run
            swap = dst;
            dst = src;
            src = swap;
        }
        long stop = read_perf_counter();
        double delta = (stop - start) / (double) nRuns;

        printf("memcpy %lld %.3f %.3f\n", localSize, delta, localSize / delta);

    }

    return 0;
}
