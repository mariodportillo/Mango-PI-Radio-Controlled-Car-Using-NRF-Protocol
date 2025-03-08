# Sample makefile for project
# Builds "myprogram.bin" from myprogram.c
# Additional source file(s): mymodule.c
# Links against libmango + mathlib

PROGRAM = myprogram.bin
SOURCES = myprogram.c mymodule.c code_extras/spi.c code_extras/i2s.c code_extras/pwm.c

all: $(PROGRAM)

# Flags for compile and link
ARCH    = -march=rv64im -mabi=lp64
ASFLAGS = $(ARCH)
CFLAGS  = $(ARCH) -g -Og -I$$CS107E/include -Icode_extras -Icode_extras/mathlib -Wall -ffreestanding -funroll-loops -ffast-math
LDFLAGS = -nostdlib -L$$CS107E/lib -Lcode_extras/mathlib -L mario_lib -T memmap.ld
LDLIBS  = -lmango -lmango_gcc -lm   # Add -lm for math functions

OBJECTS = $(addsuffix .o, $(basename $(SOURCES))) code_extras/mathlib/math_float.o

# Extract raw binary from ELF executable
%.bin: %.elf
	riscv64-unknown-elf-objcopy $< -O binary $@

# Link program executable from all object files
%.elf: $(OBJECTS) mario_lib/libmymango.a
	riscv64-unknown-elf-gcc $(LDFLAGS) $^ $(LDLIBS) -o $@

# Compile C source files
%.o: %.c
	riscv64-unknown-elf-gcc $(CFLAGS) -c $< -o $@

# Compile mathlib (ensure math_float.o is built)
code_extras/mathlib/math_float.o: code_extras/mathlib/math_float.c
	riscv64-unknown-elf-gcc $(CFLAGS) -c $< -o $@

# Assemble asm source files
%.o: %.s
	riscv64-unknown-elf-as $(ASFLAGS) $< -o $@

# Build and run the application binary
run: $(PROGRAM)
	mango-run $<

# Remove all build products
clean:
	rm -f *.o *.bin *.elf *.list *~ code_extras/mathlib/math_float.o

# Error message if a source file is missing
$(SOURCES):
	$(error cannot find source file `$@` needed for build)

libmymango.a:
	$(error cannot find libmymango.a. Change to mylib directory to build, then copy here)

.PHONY: all clean run
.PRECIOUS: %.elf %.o

# Disable built-in rules
.SUFFIXES:

export warn = -Wall -Wpointer-arith -Wwrite-strings -Werror \
              -Wno-error=unused-function -Wno-error=unused-variable \
              -fno-diagnostics-show-option
export freestanding = -ffreestanding -nostdinc \
                      -isystem $(shell riscv64-unknown-elf-gcc -print-file-name=include)
