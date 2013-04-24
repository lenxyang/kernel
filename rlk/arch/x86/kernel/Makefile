
objs=e820.c.o

build-in.o:$(objs)
	ld $(KERNEL_LDFLAGS) $? -o $@

e820.c.o: e820.c
	$(CC) $(KERNEL_CFLAGS} $? -O $@	
