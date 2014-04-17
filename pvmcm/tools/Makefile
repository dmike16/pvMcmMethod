obj-y	+= out_boundary.o out_of_grid_check.o heaviSide.o
clean-files := *~ *.o *.d

# Enable Profiling Data
#-----------------------------------------------------------------------------
ifeq ($(strip $(OPTIONS)),-pg)
cflags-y += -pg
endif