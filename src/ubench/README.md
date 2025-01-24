# Micro-Benchmark

This directory contains a micro-benchmark setup for RISC-V instructions.


# Running example in a docker container

All the examples in this directory can be built and executed using a docker container built
from https://github.com/nibrunie/rvv-examples/blob/main/riscv-toolchain.Dockerfile.
You can follow the indication of https://github.com/nibrunie/rvv-examples/blob/main/README.md for more 
information.

# How to build and execute polynomial multiplication benchmarks

Once inside a proper build/execution environment, you can change the current directory to
be `src/ubench` and execute a simple `make` command to build and run the examples:

```
make clean
make sim_ubench EXTRA_CFLAGS=" -DCOUNT_CYCLE -DNDEBUG -DNUM_TESTS=1000 -Wfatal-errors  -O3"
```

It is possible to modify the build configuration with the `EXTRA_CFLAGS` environment variable,
for example to count retired instructions rather than cycles, you can run:
```
make clean
make sim_ubench EXTRA_CFLAGS=" -DCOUNT_INSTRET -DNDEBUG -DNUM_TESTS=1000 -Wfatal-errors  -O3"
```

You can also reduce the verbosity level by removing `-DVERBOSE` and you can change
the number of tests executed for each benchmark with `-DNUM_TESTS=<num-of-tests>`.
