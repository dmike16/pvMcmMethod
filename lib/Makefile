lib-y	+= grid.o interpol_fun.o vector_copy.o built-in.o noiseRand.o
obj-y	+= object-class/
clean-files := *.o *~ *.d

# Enable Profiling Data
#-----------------------------------------------------------------------------
ifeq ($(strip $(OPTIONS)),-pg)
cflags-y += -pg
endif