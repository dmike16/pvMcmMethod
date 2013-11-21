obj-y 	+= main.o eval_ic_on_grid.o eval_method_errno.o
clean-files := *~ *.o *.d

# Enable the Mtrace algoritmic to check memory leak
#-----------------------------------------------------------------------------
ifeq ($(strip $(OPTIONS)),-d)
cflags-y += -DMTRACE
endif