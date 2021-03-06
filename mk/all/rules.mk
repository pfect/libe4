
SRCS=$(E4_COMMON_SRCS) $(E4_PUB_SRCS) $(E4_SYM_SRCS)
OBJS=$(E4_COMMON_OBJS) $(E4_PUB_OBJS) $(E4_SYM_OBJS)

all_headercpy:
	cp -rfv $(INCDIR)/* $(OUTINCDIR)/; \

all_lib: setup all_header all_headercpy $(OBJS)
	mkdir -p $(LIBDIR); \
	$(AR) $(ARFLAGS) $(LIB) $(OBJS)

all_so: setup all_header all_headercpy $(OBJS)
	mkdir -p $(LIBDIR); \
	$(CC) $(LDSOFLAGS) $(OBJS) -lc -o $(LIBSO)
	ln -sf $(LIBSO_NAME) $(LIBSO_ABI)
	ln -sf $(LIBSO_NAME) $(LIBSO_CUR)

all_dylib: setup all_header all_headercpy $(OBJS)
	$(warn WARNING: producing dylibs on macOS is an unsupported feature.)
	mkdir -p $(LIBDIR); \
	$(CC) $(LDDYLIBFLAGS) $(OBJS) -lc -o $(LIBDYLIB)

.PHONY all_header: $(BUILDDIR)/include/e4config/e4_config.h

$(BUILDDIR)/include/e4config/e4_config.h:
	echo '#define E4_MODE_ALL 1' > $@
ifeq ("$(STORE)", "none")
	echo "#define E4_STORE_NONE 1" >> $@
endif
ifeq ("$(STORE)", "mem")
	echo "#define E4_STORE_MEM 1" >> $@
endif
ifeq ("$(STORE)", "file")
	echo "#define E4_STORE_FILE 1" >> $@
endif

E4LIBS += all_lib 

all_dynamic_library: all_so
