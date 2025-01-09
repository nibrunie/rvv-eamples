# Makefile for RISC-V In a Nutshell course (RVVIAN) examples

# RISC-V C Compiler
# available options (the version used must support RVV intrinsics)
# clang/llvm
RISCVCLANG=clang  --target=riscv64

# GNU Compiler Collection (GCC)
RISCVGCC=riscv64-unknown-elf-gcc

#RISCVCCV=$(RISCVCLANG)
RISCVCCV=$(RISCVGCC)
RISCVCC=$(RISCVGCC) 

OPT=-O2

#RVV=v
RVV=_xtheadvector

#EXTRA_CFLAGS?=-DCOUNT_INSTRET
EXTRA_CFLAGS?=-DCOUNT_CYCLE

# architectural parameters for the simulation
# width of vector registers (VLEN)
VLEN?=128

# path to proxy-kernel (pk)
PK_PATH=/opt/riscv/riscv64-unknown-elf/bin/pk64 

# SIMULATOR
# Available options in the Docker (uncomment one)
SIMULATOR=spike --isa=rv64gcv_zicntr_zihpm --varch=vlen:$(VLEN),elen:64 $(PK_PATH)
# SIMULATOR=qemu-riscv64 -cpu rv64,v=on,vext_spec=v1.0,vlen=128,rvv_ta_all_1s=on

INCLUDE_DIR ?= /opt/riscv/riscv64-unknown-elf/include/
# INCLUDE=-I$(INCLUDE_DIR)

softmax_baseline.o: softmax_baseline.c
	$(RISCVCCV) $(EXTRA_CFLAGS) -I./ $(INCLUDE)  $(OPT) -march=rv64gc$(RVV) -c -o $@ $^

softmax_rvv.o: softmax_rvv.c
	$(RISCVCCV) $(EXTRA_CFLAGS) -I./ $(INCLUDE)  $(OPT) -march=rv64gc$(RVV) -c -o $@ $^

bench_softmax: bench_softmax.c softmax_baseline.o softmax_rvv.o
	 $(RISCVCC) $(EXTRA_CFLAGS) -I./ $(OPT) -march=rv64gc $^ -lm -o $@

bench_exp: bench_expf.c softmax_baseline.o softmax_rvv.o
	 $(RISCVCC) $(EXTRA_CFLAGS) -I./ $(OPT) -march=rv64gc $^ -lm -o $@

sim_bench_softmax: bench_softmax
	$(SIMULATOR) $^

sim_bench_exp: bench_exp
	$(SIMULATOR) $^

clean:
	rm -f *.o bench_softmax 

.PHONY: sim_bench_softmax clean
