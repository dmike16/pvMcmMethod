obj-y	+= pvmcm_method.o handler_sigdim.o tools/
clean-files := *~ *.o *.d

# Enable Profiling Data
#-----------------------------------------------------------------------------
ifeq ($(strip $(OPTIONS)),-pg)
cflags-y += -pg
endif