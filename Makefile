# *DOCUMENTATION*
# To see a list of typical targets execute "make help"
# Author: dmike
# Email : cipmiky@gmail.com
# This is the principal makefile of the the Semi-Lagrangian
# by mean  curve flow method program.

# Do not:
# * use make's built-in rules and variables;
# * print "Entering directory..."
MAKEFLAGS += -rR --no-print-directory


# That's the default target
PHONY  := _all
_all:

# Delete implicit rules on top Makefile
$(CURDIR)/Makefile Makefile: ;


# The _all target depend on library necessary to the program
PHONY += all
_all: all

srctree		:=$(if $(KBUILD_SRC),$(KBUILD_SRC),$(CURDIR))
objtree		:=$(CURDIR)
src		:=$(srctree)
obj		:=$(objtree)

VPATH		:= $(srctree)

export srctree objtree VPATH

# Beautify output
# ---------------------------------------------------------------------------
#
# Normally, we echo the whole command before executing it. By making
# that echo $($(quiet)$(cmd)), we now have the possibility to set
# $(quiet) to choose other forms of output instead, e.g.
#
#         quiet_cmd_cc_o_c = Compiling $(RELDIR)/$@
#         cmd_cc_o_c       = $(CC) $(c_flags) -c -o $@ $<
#
# If $(quiet) is empty, the whole command will be printed.
# If it is set to "quiet_", only the short version will be printed. 
# If it is set to "silent_", nothing will be printed at all, since
# the variable $(silent_cmd_cc_o_c) doesn't exist.
#
# A simple variant is to prefix commands with $(Q) - that's useful
# for commands that shall be hidden in non-verbose mode.
#
#	$(Q)ln $@ :<
#
#

quiet=quiet_
Q = @

ifneq ($(findstring s,$(MAKEFLAGS)),)
  quiet=silent_
endif

export quiet Q

#Look for make include files relative to root of kernel src
MAKEFLAGS += --include-dir=$(srctree)

# We need some generic definitions.
$(srctree)/scripts/Kbuild.include: ;
include $(srctree)/scripts/Kbuild.include

CC		= gcc
CFLAGS		= $(USCFLAGS) -O2 -W -Wall -pedantic -ansi -std=gnu99
AR		= ar
LD		= ld
MKDIR		= mkdir

LINUXINCLUDE	:= -Iinclude $(if $(KBUILD_SRC), -I$(srctree)/include)
SYSTEM-LIBRARIES:= -lm


export CONFIG_SHELL CC AR CFLAGS MAKE LINUXINCLUDE LD OPTIONS
export KBUILD_ARFLAGS := D

# Primary target dependencies
all: pvschema.out

core-y		:= pvmcm/
init-y		:= init/
libs-y		:= lib/
drivers-y	:= ic/

pvschema-dirs	:= $(patsubst %/,%,$(filter %/,$(init-y) \
			$(core-y) $(libs-y) $(drivers-y)))
pvschema-alldirs := $(sort $(pvschema-dirs))

init-y		:= $(patsubst %/, %/built-in.o, $(init-y))
core-y		:= $(patsubst %/, %/built-in.o, $(core-y))
drivers-y	:= $(patsubst %/, %/built-in.o, $(drivers-y))
libs-y		:= $(patsubst %/, %/lib.a, $(libs-y))

#Build pvshema
#------------------------------------------------------------------------------

pvschema-init := $(init-y)
pvschema-main := $(core-y) $(libs-y) $(drivers-y)
pvschema-all  := $(pvschema-init) $(pvschema-main)
export KBUILD_PVSCHEMA_OBJS := $(pvschema-all)

pvschema.out: $(pvschema-init) $(pvschema-main)
	$(CC) -o arch/$@ $^ $(SYSTEM-LIBRARIES)


$(sort $(pvschema-init) $(pvschema-main)): $(pvschema-dirs) ;

PHONY += $(pvschema-dirs)
$(pvschema-dirs): 
	$(Q)$(MAKE) $(build)=$@

### 
# Cleaning is done on two levels
# make clean 	Delete all the backup file from text editor
#		Leave all the objects generated
# make clobber	Delete all the .o files, the lib and the executable program

# Files removed with 'make clean'
CLEAN-FILES	= include/*~ scripts/*~
# Directories removed with 'make clobber'
CLOBBER-DIRS	= arch
CLOBBER-FILES   = $(libs-y)

# Delete only the backup files from text editor
#
clean: rm-files := $(wildcard $(CLEAN-FILES))
clean-dirs	:= $(addprefix _clean_, . $(pvschema-alldirs))

PHONY += $(clean-dirs) clean
$(clean-dirs):
	$(Q)$(MAKE) $(clean)=$(patsubst _clean_%,%,$@)

clean: $(clean-dirs)
	$(call cmd,rmfiles)
# Delete all the files generated
#
clobber: rm-dirs := $(wildcard $(CLOBBER-DIRS))
clobber: rm-files:= $(wildcard $(CLOBBER-FILES))

PHONY += clobber
clobber: clean
	$(call cmd,rmfiles)
	$(call cmd,rmdirs)

# Brief documentation of the typical targets used
# ---------------------------------------------------------------------------

help:
	@echo  'Cleaning targets:'
	@echo  '  clean		  - Remove backup files by text editor + object files + lib'
	@echo  '  clobber	  - make clean + remove the executable program'
	@echo  ''
	@echo  'Other generic targets:'
	@echo  '  all		  - Build all targets marked with '
	@echo  'pvschema	  - Build the executable program'
	@echo  ''

#  Debug the programm to test if there are memory leaks
#----------------------------------------------------------------------------

PHONY += debug-memory

debug-memory :
	@$(MAKE) OPTIONS=-d
	export MALLOC_TRACE=memory.log
	./arch/pvschema.out ini.dat && mtrace arch/pvschema.out $$MALLOC_TRACE

#===============================================================================

quiet_cmd_rmdirs = $(if $(wildcard $(rm-dirs)),CLEAN   $(wildcard $(rm-dirs)))
      cmd_rmdirs = rm -rf $(rm-dirs)

quiet_cmd_rmfiles = $(if $(wildcard $(rm-files)),CLEAN   $(wildcard $(rm-files)))
      cmd_rmfiles = rm -f $(rm-files)

# Create output directory if does not exist
ifneq ($(MAKECMDGOALS),clean)
_create-out-dir :=    \
	$(if $(call file-exists,  \
	arch),, \
	$(shell $(MKDIR) arch))
endif

# Shorthand for $(Q)$(MAKE) -f scripts/Makefile.clean obj=dir
# Usage:
# $(Q)$(MAKE) $(clean)=dir
clean := -f $(if $(KBUILD_SRC),$(srctree)/)scripts/Makefile.clean obj

.PHONY : $(PHONY)