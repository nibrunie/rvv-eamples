#include <stddef.h>
#include "riscv_vector.h"

typedef struct {
    unsigned long perf_count;
    int elt_per_op;
    int errors;
} ubench_result_t;

static ubench_result_t accumulate_ubench_result(ubench_result_t res, ubench_result_t new_result) {
  res.perf_count += new_result.perf_count;
  res.elt_per_op  = new_result.elt_per_op;
  res.errors     |= new_result.errors;

  return res;
}

/** function type for micro-benchmarks */
typedef ubench_result_t (bench_func_t)(size_t);

/** function type for micro-benchmarks with customizable input data */
typedef ubench_result_t (bench_data_func_t)(size_t, uint64_t []);

/** function type for customizable data generator */
typedef void (data_gen_t)(uint64_t[]);

/** Descriptor structure for basic micro-benchmark */
typedef struct {
    bench_func_t* bench;
    ubench_result_t result;
    char label[256];
} ubench_t;

/** Descriptor structure for micro-benchmark with custom data generation */
typedef struct {
    bench_data_func_t* bench;
    data_gen_t* data_gen;
    ubench_result_t result;
    char label[256];
} ubench_data_t;

#define BENCH_0OP_INSN(op) \
ubench_result_t bench_lat_##op(size_t n) { \
    long start = read_perf_counter(); \
    for (int i = 0; i < n / 16; i++) { \
        asm volatile( \
            #op "\n" \
            #op "\n" \
            #op "\n" \
            #op "\n" \
            #op "\n" \
            #op "\n" \
            #op "\n" \
            #op "\n" \
            #op "\n" \
            #op "\n" \
            #op "\n" \
            #op "\n" \
            #op "\n" \
            #op "\n" \
            #op "\n" \
            #op "\n" \
        : \
        : \
        : \
        ); \
    } \
    long stop = read_perf_counter(); \
    return (ubench_result_t){ \
        .perf_count = (stop - start), \
        .elt_per_op = 1, \
        .errors = 0 \
    }; \
}\

/** Build a latency benchmark for a 2-operand instruction
 *
 * Latency is measure by building a chain of dependent instructions
 */
#define BENCH_LAT_2OP_INSN(op) \
ubench_result_t bench_lat_##op(size_t n) { \
    size_t cnt = n / 16; \
    long start = read_perf_counter(); \
    asm volatile( \
        "li t0, 7\n" \
        "li t2, 0xcafebebe1337beef\n" \
        "mv t1, t2\n" \
    "1:\n" \
        #op " t1, t1, t0\n" \
        #op " t1, t1, t0\n" \
	    "add t1, t2, t1\n"\
        #op " t1, t1, t0\n" \
        #op " t1, t1, t0\n" \
	    "add t1, t2, t1\n"\
        #op " t1, t1, t0\n" \
        #op " t1, t1, t0\n" \
	    "add t1, t2, t1\n"\
        #op " t1, t1, t0\n" \
        #op " t1, t1, t0\n" \
	    "add t1, t2, t1\n"\
        #op " t1, t1, t0\n" \
        #op " t1, t1, t0\n" \
	    "add t1, t2, t1\n"\
        #op " t1, t1, t0\n" \
        #op " t1, t1, t0\n" \
	    "add t1, t2, t1\n"\
        #op " t1, t1, t0\n" \
        #op " t1, t1, t0\n" \
	    "add t1, t2, t1\n"\
        #op " t1, t1, t0\n" \
        #op " t1, t1, t0\n" \
        "addi %[cnt], %[cnt], -1\n" \
        "bnez %[cnt], 1b\n" \
    : [cnt]"+r"(cnt) \
    : \
    : "t0", "t1", "t2"\
    ); \
    long stop = read_perf_counter(); \
    long delta = stop - start; \
    cnt = n / 16; \
    start = read_perf_counter(); \
    asm volatile( \
        "li t0, 7\n" \
        "li t1, 0xcafebebe1337beef\n" \
        "li t2, 0xbeefcafebebe1337\n" \
    "1:\n" \
	    "add t1, t2, t1\n"\
	    "add t1, t2, t1\n"\
	    "add t1, t2, t1\n"\
	    "add t1, t2, t1\n"\
	    "add t1, t2, t1\n"\
	    "add t1, t2, t1\n"\
	    "add t1, t2, t1\n"\
        "addi %[cnt], %[cnt], -1\n" \
        "bnez %[cnt], 1b\n" \
    : [cnt]"+r"(cnt) \
    : \
    : "t0", "t1", "t2"\
    ); \
    stop = read_perf_counter(); \
    long correction = (stop - start); \
    return (ubench_result_t){ \
        .perf_count = (delta - correction), \
        .elt_per_op = 1, \
        .errors = 0 \
    }; \
}\


#define BENCH_THROUGHPUT_2OP_INSN(op) \
ubench_result_t bench_throughput_##op##_values(size_t n, uint64_t v[2]) { \
    long start = read_perf_counter(); \
    size_t cnt = n / 13; \
    asm volatile( \
        "mv a0, %[v0]\n" \
        "mv a1, %[v1]\n" \
    "1:\n" \
        #op " a2, a1, a0\n" \
        #op " a3, a1, a0\n" \
        #op " a4, a1, a0\n" \
        #op " a5, a1, a0\n" \
        #op " a6, a1, a0\n" \
        #op " a7, a1, a0\n" \
        #op " t0, a1, a0\n" \
        #op " t1, a1, a0\n" \
        #op " t2, a1, a0\n" \
        #op " t3, a1, a0\n" \
        #op " t4, a1, a0\n" \
        #op " t5, a1, a0\n" \
        #op " t6, a1, a0\n" \
        "addi %[cnt], %[cnt], -1\n" \
        "bnez %[cnt], 1b\n" \
    : [cnt]"+r"(cnt) \
    : [v0]"r"(v[0]), [v1]"r"(v[1]) \
    : "a0", "a1", "a2", "a3", "a4", \
      "a5", "a6", "a7", \
      "t0", "t1", "t2", "t3", "t4", \
      "t5", "t6" \
    ); \
    long stop = read_perf_counter(); \
    return (ubench_result_t){ \
        .perf_count = (stop - start), \
        .elt_per_op = 1, \
        .errors = 0 \
    }; \
}\
ubench_result_t bench_throughput_##op(size_t n) { \
    uint64_t data[2] = {3, 0xcafebebe1337beefull}; \
    return bench_throughput_##op##_values(n, data); \
}\

#define BENCH_LAT_INSN_TC(op) (ubench_t){.bench = bench_lat_##op,                .label= "latency " #op }
#define BENCH_THROUGHPUT_INSN_TC(op) (ubench_t){.bench = bench_throughput_##op,  .label= "throughput " #op}


// #define BENCH_THROUGHPUT_2OP_FPD_INSN(op) 

#define BENCH_THROUGHPUT_2OP_FPD_INSN(op) BENCH_THROUGHPUT_2OP_FP_INSN(op, d, 0x3fcdbeef3fcdbeefull, 0x4abdcafe4abdcafeull)
#define BENCH_THROUGHPUT_2OP_FPS_INSN(op) BENCH_THROUGHPUT_2OP_FP_INSN(op, s, 0xffffffff3fcdbeefull, 0xffffffff4abdcafeull)

#define BENCH_THROUGHPUT_2OP_FP_INSN(op, fmt_suffix, init_ft0, init_ft1) \
ubench_result_t bench_throughput_##op##_##fmt_suffix##_values(size_t n, uint64_t v0, uint64_t v1) { \
    size_t cnt = n / 13; \
    long start = read_perf_counter(); \
    asm volatile( \
        "fmv." #fmt_suffix ".x fa0, %[v0]\n" \
        "fmv." #fmt_suffix ".x fa1, %[v1]\n" \
    "1:\n" \
        #op "." #fmt_suffix " fa2, fa1, fa0\n" \
        #op "." #fmt_suffix " fa3, fa1, fa0\n" \
        #op "." #fmt_suffix " fa4, fa1, fa0\n" \
        #op "." #fmt_suffix " fa5, fa1, fa0\n" \
        #op "." #fmt_suffix " fa6, fa1, fa0\n" \
        #op "." #fmt_suffix " fa7, fa1, fa0\n" \
        #op "." #fmt_suffix " ft0, fa1, fa0\n" \
        #op "." #fmt_suffix " ft1, fa1, fa0\n" \
        #op "." #fmt_suffix " ft2, fa1, fa0\n" \
        #op "." #fmt_suffix " ft3, fa1, fa0\n" \
        #op "." #fmt_suffix " ft4, fa1, fa0\n" \
        #op "." #fmt_suffix " ft5, fa1, fa0\n" \
        #op "." #fmt_suffix " ft6, fa1, fa0\n" \
        "addi %[cnt], %[cnt], -1\n" \
        "bnez %[cnt], 1b\n" \
    : [cnt]"+r"(cnt) \
    : [v0]"r"(v0), [v1]"r"(v1) \
    : "fa0", "fa1", "fa2", "fa3", "fa4", \
      "fa5", "fa6", "fa7", \
      "ft0", "ft1", "ft2", "ft3", "ft4", \
      "ft5", "ft6" \
    ); \
    long stop = read_perf_counter(); \
    cnt = n / 13; \
    long delta = stop - start; \
    start = read_perf_counter(); \
    asm volatile( \
        "li t0, 0x3fcdbeef3fcdbeef\n" \
        "li t1, 0x4abdcafe4abdcafe\n" \
        "fmv." #fmt_suffix ".x fa0, t0\n" \
        "fmv." #fmt_suffix ".x fa1, t1\n" \
    "1:\n" \
        "addi %[cnt], %[cnt], -1\n" \
        "bnez %[cnt], 1b\n" \
    : [cnt]"+r"(cnt) \
    : \
    : "fa0", "fa1", "fa2", "fa3", "fa4", \
      "fa5", "fa6", "fa7", \
      "ft0", "ft1", "ft2", "ft3", "ft4", \
      "ft5", "ft6" \
    ); \
    stop = read_perf_counter(); \
    long correction = stop - start; \
    return (ubench_result_t){ \
        .perf_count = (delta - correction), \
        .elt_per_op = 1, \
        .errors = 0 \
    }; \
}\
ubench_result_t bench_throughput_##op##_##fmt_suffix(size_t n) { \
    return bench_throughput_##op##_##fmt_suffix##_values(n, init_ft0, init_ft1); \
} \

/** Build a latency benchmark for a 2-operand floating-point instruction
 *
 * Latency is measured by building a chain of dependent instructions
 */
#define BENCH_LAT_2OP_FPD_INSN(op) BENCH_LAT_2OP_FP_INSN(op, d, "0x3fcdbeef3fcdbeef", "0x4abdcafe4abdcafe")
#define BENCH_LAT_2OP_FPS_INSN(op) BENCH_LAT_2OP_FP_INSN(op, s, "0xffffffff3fcdbeef", "0xffffffff4abdcafe")

#define BENCH_LAT_2OP_FP_INSN(op, fmt_suffix, init_ft0, init_ft1) \
ubench_result_t bench_lat_##op##_##fmt_suffix(size_t n) { \
    size_t cnt = n / 16; \
    long start = read_perf_counter(); \
    asm volatile( \
        "li t0, " init_ft0 "\n" \
        "li t2, " init_ft1 "\n" \
        "fmv." #fmt_suffix ".x ft0, t0\n" \
        "fmv." #fmt_suffix ".x ft1, t2\n" \
        "fmv." #fmt_suffix ".x ft2, t2\n" \
    "1:\n" \
        #op "." #fmt_suffix " ft1, ft1, ft0\n" \
        #op "." #fmt_suffix " ft1, ft1, ft0\n" \
        #op "." #fmt_suffix " ft1, ft1, ft0\n" \
        #op "." #fmt_suffix " ft1, ft1, ft0\n" \
        #op "." #fmt_suffix " ft1, ft1, ft0\n" \
        #op "." #fmt_suffix " ft1, ft1, ft0\n" \
        #op "." #fmt_suffix " ft1, ft1, ft0\n" \
        #op "." #fmt_suffix " ft1, ft1, ft0\n" \
        #op "." #fmt_suffix " ft1, ft1, ft0\n" \
        #op "." #fmt_suffix " ft1, ft1, ft0\n" \
        #op "." #fmt_suffix " ft1, ft1, ft0\n" \
        #op "." #fmt_suffix " ft1, ft1, ft0\n" \
        #op "." #fmt_suffix " ft1, ft1, ft0\n" \
        #op "." #fmt_suffix " ft1, ft1, ft0\n" \
        #op "." #fmt_suffix " ft1, ft1, ft0\n" \
        #op "." #fmt_suffix " ft1, ft1, ft0\n" \
        "addi %[cnt], %[cnt], -1\n" \
        "bnez %[cnt], 1b\n" \
    : [cnt]"+r"(cnt) \
    : \
    : "ft0", "ft1", "ft2"\
    ); \
    long stop = read_perf_counter(); \
    long delta = stop - start; \
    cnt = n / 16; \
    start = read_perf_counter(); \
    asm volatile( \
        "li t0, 0x3fcdbeef3fcdbeef\n" \
        "li t2, 0x4abdcafe4abdcafe\n" \
        "fmv.d.x ft0, t0\n" \
        "fmv.d.x ft1, t2\n" \
        "fmv.d.x ft2, t2\n" \
    "1:\n" \
        "addi %[cnt], %[cnt], -1\n" \
        "bnez %[cnt], 1b\n" \
    : [cnt]"+r"(cnt) \
    : \
    : \
    ); \
    stop = read_perf_counter(); \
    long correction = (stop - start); \
    return (ubench_result_t){ \
        .perf_count = (delta - correction), \
        .elt_per_op = 1, \
        .errors = 0 \
    }; \
}


#define BENCH_LAT_VEC_INSN_TC(op, LMUL, elt, suffix) (ubench_t){.bench = bench_lat_##op##_m##LMUL##_e##elt##_##suffix, .label= "latency " #op " m" #LMUL " e" #elt " " #suffix}
#define BENCH_THROUGHPUT_VEC_INSN_TC(op, LMUL, elt, suffix) (ubench_t){.bench = bench_throughput_##op##_m##LMUL##_e##elt##_##suffix, .label= "throughput " #op " m" #LMUL " e" #elt " " #suffix}


#define BENCH_LAT_2OP_SUFFIX_VEC_INSN(op, LMUL, elt, suffix) \
ubench_result_t bench_lat_##op##_m##LMUL##_e##elt##_##suffix(size_t n) { \
    size_t avl = 65536; \
    size_t cnt = n / 16; \
    size_t vl = 0; \
    long start = read_perf_counter(); \
    asm volatile( \
        "vsetvli t0, x0, e32, m" #LMUL ", ta, ma\n" \
        "li t0, 0x3c003c00\n" \
        "vmv.v.x v8, t0\n" \
        "vsetvli %[vl], %[avl], e" #elt ", m" #LMUL ", ta, ma\n" \
        "vid.v v16\n" \
        "vor.vv v16, v16, v8\n" \
        "vid.v v24\n" \
        "vor.vv v24, v24, v8\n" \
    "1:\n" \
        #op "." #suffix " v8, v16, v24\n" \
        #op "." #suffix " v8, v16, v24\n" \
        #op "." #suffix " v8, v16, v24\n" \
        #op "." #suffix " v8, v16, v24\n" \
        #op "." #suffix " v8, v16, v24\n" \
        #op "." #suffix " v8, v16, v24\n" \
        #op "." #suffix " v8, v16, v24\n" \
        #op "." #suffix " v8, v16, v24\n" \
        #op "." #suffix " v8, v16, v24\n" \
        #op "." #suffix " v8, v16, v24\n" \
        #op "." #suffix " v8, v16, v24\n" \
        #op "." #suffix " v8, v16, v24\n" \
        #op "." #suffix " v8, v16, v24\n" \
        #op "." #suffix " v8, v16, v24\n" \
        #op "." #suffix " v8, v16, v24\n" \
        #op "." #suffix " v8, v16, v24\n" \
        "addi %[cnt], %[cnt], -1\n" \
        "bnez %[cnt], 1b\n" \
    : [cnt]"+r"(cnt), [vl]"+r"(vl), [avl]"+r"(avl) \
    : \
    : "t0" \
    ); \
    long stop = read_perf_counter(); \
    long delta = stop - start; \
    cnt = n / 16; \
    start = read_perf_counter(); \
    asm volatile( \
        "vsetvli t0, x0, e" #elt ", m" #LMUL ", ta, ma\n" \
        "vid.v v16\n" \
        "vid.v v24\n" \
    "1:\n" \
        "addi %[cnt], %[cnt], -1\n" \
        "bnez %[cnt], 1b\n" \
    : [cnt]"+r"(cnt) \
    : \
    : \
    ); \
    stop = read_perf_counter(); \
    long correction = stop - start; \
    return (ubench_result_t){ \
        .perf_count = (delta - correction), \
        .elt_per_op = vl, \
        .errors = 0 \
    }; \
}

#define BENCH_THROUGHPUT_2OP_VV_VEC_INSN(op, LMUL, elt) BENCH_THROUGHPUT_2OP_SUFFIX_VEC_INSN(op, LMUL, elt, vv) 
#define BENCH_THROUGHPUT_2OP_VS_VEC_INSN(op, LMUL, elt) BENCH_THROUGHPUT_2OP_SUFFIX_VEC_INSN(op, LMUL, elt, vs)
#define BENCH_THROUGHPUT_2OP_WV_VEC_INSN(op, LMUL, elt) BENCH_THROUGHPUT_2OP_SUFFIX_VEC_INSN(op, LMUL, elt, wv)

#define BENCH_THROUGHPUT_2OP_SUFFIX_VEC_INSN(op, LMUL, elt, suffix) \
ubench_result_t bench_throughput_##op##_m##LMUL##_e##elt##_##suffix(size_t n) { \
    size_t cnt = n / 18; \
    size_t vl = 0; \
    long start = read_perf_counter(); \
    asm volatile( \
        "vsetvli t0, x0, e32, m" #LMUL ", ta, ma\n" \
        "li t0, 0x3c003c00\n" \
        "vmv.v.x v8, t0\n" \
        "vsetvli %[vl], x0, e" #elt ", m" #LMUL ", ta, ma\n" \
        "vid.v v16\n" \
        "vor.vv v16, v16, v8\n" \
        "vid.v v24\n" \
        "vor.vv v24, v24, v8\n" \
    "1:\n" \
        #op "." #suffix " v4,  v0,  v8\n" \
        #op "." #suffix " v12, v16, v20\n" \
        #op "." #suffix " v24, v28, v0\n" \
        #op "." #suffix " v8,  v0,  v20\n" \
        #op "." #suffix " v16, v4,  v28\n" \
        #op "." #suffix " v24, v28, v0\n" \
        #op "." #suffix " v4,  v0,  v8\n" \
        #op "." #suffix " v12, v16, v20\n" \
        #op "." #suffix " v24, v28, v0\n" \
        #op "." #suffix " v8,  v0,  v20\n" \
        #op "." #suffix " v16, v4,  v28\n" \
        #op "." #suffix " v24, v28, v0\n" \
        #op "." #suffix " v4,  v0,  v8\n" \
        #op "." #suffix " v12, v16, v20\n" \
        #op "." #suffix " v24, v28, v0\n" \
        #op "." #suffix " v8,  v0,  v20\n" \
        #op "." #suffix " v16, v4,  v28\n" \
        #op "." #suffix " v24, v28, v0\n" \
        "addi %[cnt], %[cnt], -1\n" \
        "bnez %[cnt], 1b\n" \
    : [cnt]"+r"(cnt), [vl]"+r"(vl) \
    : \
    : "t0" \
    ); \
    long stop = read_perf_counter(); \
    long delta = stop - start; \
    cnt = n / 16; \
    start = read_perf_counter(); \
    asm volatile( \
        "vsetvli t0, x0, e" #elt ", m" #LMUL ", ta, ma\n" \
        "vid.v v16\n" \
        "vid.v v24\n" \
    "1:\n" \
        "addi %[cnt], %[cnt], -1\n" \
        "bnez %[cnt], 1b\n" \
    : [cnt]"+r"(cnt) \
    : \
    : \
    ); \
    stop = read_perf_counter(); \
    long correction = stop - start; \
    return (ubench_result_t){ \
        .perf_count = (delta - correction), \
        .elt_per_op = vl, \
        .errors = 0 \
    }; \
}
