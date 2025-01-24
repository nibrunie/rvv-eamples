# Makefile for RISC-V In a Nutshell course (RVVIAN) examples

# RISC-V C Compiler
# available options (the version used must support RVV intrinsics)
# clang/llvm
RISCVCLANG ?= clang  --target=riscv64
# RISCVCC=clang  --target=riscv64
# GNU Compiler Collection (GCC)
RISCVCC ?= riscv64-unknown-elf-gcc 
OBJDUMP ?= riscv64-unknown-elf-objdump

EXTRA_CFLAGS?=-DCOUNT_INSTRET

# architectural parameters for the simulation
# width of vector registers (VLEN)
VLEN?=128

# path to proxy-kernel (pk)
PK_PATH=/opt/riscv/riscv64-unknown-elf/bin/pk64 

BUILD_ARCH=rv64gcv
SIM_ARCH=rv64gcv

# SIMULATOR
# Available options in the Docker (uncomment one)
SIMULATOR=spike --isa=$(SIM_ARCH)_zicntr_zihpm --varch=vlen:$(VLEN),elen:64 $(PK_PATH)
# SIMULATOR=qemu-riscv64 -cpu rv64,v=on,vext_spec=v1.0,vlen=128,rvv_ta_all_1s=on

COMMON_INCLUDES = -I./ -I../utils/

INCLUDE_DIR ?= /opt/riscv/riscv64-unknown-elf/include/
BUILD_DIR ?= $(PWD)/build/

$(BUILD_DIR)/ubench: microbenchmarks.c
	mkdir -p $(BUILD_DIR)
	$(RISCVCC) $(EXTRA_CFLAGS) $(COMMON_INCLUDES) -O2 -march=rv64gcv $^ -lm -o $@
	$(OBJDUMP) -d $@ > $(BUILD_DIR)/ubench.dump


sim_ubench: $(BUILD_DIR)/ubench
	$(SIMULATOR) $^

clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/ubench

.PHONY: sim_ubench clean
