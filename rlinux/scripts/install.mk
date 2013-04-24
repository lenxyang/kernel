
install: vmlinux
	dd if=bootsect of=$vm/a.img bs=512 count=1 conv=notrunc seek=0
	dd if=vmlinux of=$vm/a.img bs=512 count=500 conv=notrunc seek=1

