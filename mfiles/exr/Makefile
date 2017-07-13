USE_GCC = 1
DEBUG_MODE = 1

CFLAGS =
LDFLAGS =
INCLUDES = -I../include
LIBS =

ifeq ($(USE_GCC), 1)
	include ../gcc.mk
else
	include ../icc.mk
endif

include ../matlab.mk
include ../mex_utils.mk
include openexr.mk

#all: read write
all: read write get is test

get: exrget.$(MEXEXT)
read: exrread.$(MEXEXT)
write: exrwrite.$(MEXEXT)
is: isexr.$(MEXEXT)
test: test_exr.$(MEXEXT)

%.$(MEXEXT): %.o exr.o
	$(LD) $(LDFLAGS) $(INCLUDES) $(CFLAGS) -o $@ $^ $(LIBS) 

%.o: %.cpp exr.h
	$(CC) $(INCLUDES) $(LDFLAGS) $(CFLAGS) -c -o $@ $<
	
clean:
	rm -rf *.o *~

distclean:	
	rm -rf *.o *~ *.$(MEXEXT)
