# Makefile to build the R^n linear interpolation
# Author: dmike
# Email : cipmiky@gmail.com
# This is part of a large program, that is based on the Semi-Lagrangian 
# by mean  curve flow method.

vpath %.c src
vpath %.h include

CFLAGS = -I include -Wall -W -ansi -pedantic -std=gnu99 -c 
CC = gcc
SOURCE := inter_P1.c grid.c u0.c
objs = inter_P1.o grid.o u0.o
lib := -lm

define print-msg 
@echo "#\n#    Makefile to Build P1 \n#"
@echo "#  Author  : dmike "
@echo "#  Email   : cipmiky@gmail.com "
@echo "#  Licence : open source \n#"
endef

ifeq ($(strip $(OPTIONS)),-d)
CFLAGS += -DMTRACE
endif

.PHONY : all

all: build_msg p1

.PHONY : build_msg

build_msg :
	$(print-msg)

p1 : $(objs)
	$(CC) $^ -o $@ $(lib)

inter_P1.o grid.o u0.o :

-include $(subst .c,.d,$(SOURCE))


%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\).o[ :]*,\1.o $@ : ,g' <$@.$$$$ > $@; \
	rm -f $@.$$$$

.PHONY : clean

clean :
	-rm -rf p1 *~ *.d *.o src/*~ include/*~

# help -

.PHONY: debug-memory

debug-memory :
	export MALLOC_TRACE=memory.log
	@$(MAKE) OPTIONS=-d
	./p1 ini.dat
	mtrace p1 $(MALLOC_TRACE)