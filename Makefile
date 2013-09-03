# Makefile to build the R^n linear interpolation
# Author: dmike
# Email : cipmiky@gmail.com
# This is part of a large program, that is based on the Semi-Lagrangian 
# by mean  curve flow method.

vpath %.c src
vpath %.h include

CFLAGS = -I include -Wall -W -ansi -pedantic -std=gnu99 -c 
CC = gcc
SOURCE := $(wildcard src/*.c)
objs := $(subst .c,.o,$(SOURCE)) 
lib := -lm

define print-msg 
@echo "#\n#    Makefile to Build P1 \n#"
@echo "#  Author  : dmike "
@echo "#  Email   : cipmiky@gmail.com "
@echo "#  Licence : open source \n#"
endef

define make-depend
  $(CC)  $(CFLAGS) -MM $1 | \
  sed 's,\($(notdir $2)\) *:,$2 $3: ,' > $3.tmp
  sed -e 's/#.*//' \
      -e 's/^[^:]*: *//' \
      -e 's/ *\\$$$$//'  \
      -e '/^$$$$/ d'     \
      -e 's/$$$$/ :/'    \
      -e 's/$$/:/' $3.tmp >> $3.tmp
  mv $3.tmp $3
endef

ifeq ($(strip $(OPTIONS)),-d)
CFLAGS += -DMTRACE
endif

PHONY := all

all: build_msg p1

PHONY += build_msg

build_msg :
	$(print-msg)

p1 : $(objs)
	$(CC) $^ -o $@ $(lib)

$(objs) :

ifneq ($(MAKECMDGOALS),clean)
 -include $(subst .c,.d,$(SOURCE))
endif


%.o: %.c
	$(call make-depend,$<,$@,$(subst .o,.d,$@))
	$(CC) $(CFLAGS) -o $@ $<

PHONY += clean

clean :
	-rm -rf p1 *~ src/*.d src/*.o src/*~ include/*~

# help -

PHONY += debug-memory

debug-memory :
	export MALLOC_TRACE=memory.log
	@$(MAKE) OPTIONS=-d
	./p1 ini.dat
	mtrace p1 $(MALLOC_TRACE)

.PHONY : $(PHONY)