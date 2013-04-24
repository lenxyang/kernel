yesroot=arch/i386/kernel
out=out/${root}

objs=${out}/head.S.o

${out}/vmlinux : ${objs}
	${LD} -s -N -T ${root}/vmlinux.lds $? -o $@
	${LD} -N -T ${root}/vmlinux.lds $? -o $@.gdb

${out}/head.S.o : ${root}/head.S.o
	${CC} ${aflags} -c $< -o $@

${out}:
	@if [ ! -x ${out} ]; then mkdir -p ${out}; fi

export vmlinux=${out}/vmlinux
