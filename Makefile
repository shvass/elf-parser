# This file is a part of elf-parser
# Copyright (C) Copyright (C) 2025  akshay bansod <akbansd@gmail.com>

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

# Makefile to produce loadable overlays

# prefix toolchain for cross-compilation
PREFIX ?= xtensa-$(IDF_TARGET)-elf-

CC := $(PREFIX)gcc
CXX := $(PREFIX)g++
LD := $(PREFIX)ld
AS := $(PREFIX)as
AR := $(PREFIX)ar
OBJCOPY := $(PREFIX)objcopy
OBJDUMP := $(PREFIX)objdump
SIZE := $(PREFIX)size
NM := $(PREFIX)nm




CFLAGS = -FPIC -shared         # compile a position independent object
CFLAGS += -fvisibility=hidden  # add default symbol visibility of hidden
CFLAGS += -I.                  # include current directory


LDFLAGS = -nostdlib            # use no standard library
LDFLAGS += -Wl,--strip-all     # strip all sectors
LDFLAGS += -Wl,-e_start        # the object is being compiled as a shared library, force add an entrypoint
LDFLAGS += -Wl,-z,relro,-z,now # enable relro; disabling wink binding
LDFLAGS += -fno-plt            # disable plt

# LDFLAGS +=  /lib/crt1.o      # link with c startup lib


# read info from objs
%.log : %.out
	readelf -a -S $^ >  $@
	$(OBJDUMP) $^ -d >> $@


%.hh : %
	@echo "converting object ${%.o} to c array header ${%.hh}"
	xxd -i $^ > $@



clean:
	rm *.o *.log *.hh