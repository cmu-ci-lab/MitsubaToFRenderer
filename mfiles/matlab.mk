MATLABDIR = /usr/local/MATLAB/R2017a
MATLABARCH = glnxa64
MEXEXT = $(shell $(MATLABDIR)/bin/mexext)
MAPFILE = mexFunction.map

MATLABLIBS = -L$(MATLABDIR)/bin/$(MATLABARCH) -lmx -lmex -lmat
RPATH = -Wl,-rpath-link,$(MATLABDIR)/bin/$(MATLABARCH)
LIBS += $(RPATH) $(MATLABLIBS)

MATLABINCLUDE = -I$(MATLABDIR)/extern/include
INCLUDES += $(MATLABINCLUDE)

MEXFLAGS = -DMATLAB_MEX_FILE -D_GNU_SOURCE -fexceptions -fno-omit-frame-pointer
CFLAGS += $(MEXFLAGS)
LDFLAGS += -pthread -shared -Wl,--version-script,$(MATLABDIR)/extern/lib/$(MATLABARCH)/$(MAPFILE) -Wl,--no-undefined
