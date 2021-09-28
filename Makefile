CC=gcc
CFLAGS=-I.
DEPS = register.h prefix.h modrm.h sib.h op.h label.h list.h instr.h lin.h

disasm: register.c prefix.c modrm.c sib.c op.c label.c list.c instr.c lin.c main.c $(DEPS)
	$(CC) register.c prefix.c modrm.c sib.c op.c label.c list.c instr.c lin.c main.c -o disasm

clean:
	rm -f disasm
