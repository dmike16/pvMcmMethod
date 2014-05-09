obj-y	+= pvmcm_method.o mcmschema.o handler_sigdim.o tools/
clean-files := *~ *.o *.d

# Enable Profiling Data
#-----------------------------------------------------------------------------
ifeq ($(strip $(OPTIONS)),-pg)
cflags-y += -pg
endif