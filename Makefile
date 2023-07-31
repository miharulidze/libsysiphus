CXX=g++
RELEASE_FLAGS= -O3
LIBSYSIPHUS_OBJS = libsysiphus.po
LIBSYSIPHUS_CXX_FLAGS = -fPIC

%.po: %.cpp
	$(CXX) $(LIBSYSIPHUS_CXX_FLAGS) -o $@ -c $<

libsysiphus.so: $(LIBSYSIPHUS_OBJS)
	$(CXX) $(LIBSYSIPHUS_CXX_FLAGS) -shared -o $@ $(LIBSYSIPHUS_OBJS)

release: LIBSYSIPHUS_CXX_FLAGS += -O3
release: libsysiphus.so

clean:
	rm *.po libsysiphus.so
