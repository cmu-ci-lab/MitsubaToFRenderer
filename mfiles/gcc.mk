CC = g++
LD = g++
GENERALFLAGS = -fPIC -Wall -Wextra -Wpedantic -g -std=c++11 -fdiagnostics-color=always
EXTRAWARNINGFLAGS = -Wdouble-promotion -Wswitch-default -Wuninitialized -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn -Wsuggest-attribute=format -Wfloat-equal -Wtrampolines -Wundef -Wunsafe-loop-optimizations -Wtype-limits -Wcast-qual -Wcast-align -Wconversion -Wzero-as-null-pointer-constant -Wuseless-cast -Wlogical-op -Wmissing-declarations -Wopenmp-simd -Wpacked -Wpacked-bitfield-compat -Wredundant-decls -Wlong-long -Winvalid-pch
#-Wsign-conversion
#-Waggregate-return 
#-Wpadded
OPTIMFLAGS = -march=native -Ofast -fopenmp -pthread -pipe -flto -fuse-linker-plugin -fuse-ld=gold -fno-exceptions -fvisibility-inlines-hidden
#-Wvector-operation-performance
#-fno-rtti
#-funsafe-loop-optimizations
#-fstrict-aliasing
#'-ftree-vectorize', '-mfpmath=sse', '-funsafe-math-optimizations', '-fno-rounding-math', '-fno-signaling-nans', '-fno-math-errno', '-fomit-frame-pointer'
REPORTSFLAGS = -Winline -Wimplicit
DEBUGFLAG = -g
CFLAGS += $(DEBUGFLAG) $(GENERALFLAGS) $(OPTIMFLAGS)
ifeq ($(DEBUG_MODE), 0)
	CFLAGS += -DNDEBUG
endif
ifeq ($(PRODUCE_REPORTS), 1)
	CFLAGS += $(REPORTSFLAGS) 
endif
