obj-y 	+= main.o
clean-files := *~ *.o *.d

# Enable the Mtrace algoritmic to check memory leak
#-----------------------------------------------------------------------------
ifeq ($(strip $(OPTIONS)),-d)
ccflags-y += -DMTRACE
endif