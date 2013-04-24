
root=arch/i386/boot
out=out/${root}

${out}/bootsect.bin : ${out}/bootsect.S.o
	${LD} --oformat binary -Ttext 0x7c00 $< -o $@
	${LD} -N -Ttext 0x7c00 $< -o ${out}/bootsect.gdb

${out}/bootsect.S.o : ${root}/bootsect.S ${out}
	${CC} ${AFLAGS} -g -c $< -o $@
${out}:
	@if [ ! -x ${out} ]; then mkdir -p ${out}; fi

export bootsect=${out}/bootsect.bin
