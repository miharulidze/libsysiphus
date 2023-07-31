CC=gcc
RELEASE_FLAGS= -O3
LIBSYSIPHUS_OBJS = libsysiphus.o
LIBSYSIPHUS_CFLAGS = -fPIC

%.o: %.c
	$(CC) $(LIBSYSIPHUS_CFLAGS) -o $@ -c $<

libsysiphus.so: $(LIBSYSIPHUS_OBJS)
	$(CC) $(LIBSYSIPHUS_CFLAGS) -shared -o $@ $(LIBSYSIPHUS_OBJS)

release: LIBSYSIPHUS_CFLAGS += -O3
release: libsysiphus.so

clean:
	rm *.o libsysiphus.so
