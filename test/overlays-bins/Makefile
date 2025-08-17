
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
CFLAGS += -mlongcalls                  # include current directory


LDFLAGS = -nostdlib            # use no standard library
LDFLAGS += -Wl,--strip-all     # strip all sectors
LDFLAGS += -Wl,-e_start        # the object is being compiled as a shared library, force add an entrypoint
LDFLAGS += -Wl,-z,relro,-z,now # enable relro; disabling wink binding
LDFLAGS += -fno-plt            # disable plt
# LDFLAGS += -r            # disable plt
# LDFLAGS += -Wl,--unresolved-symbols=ignore-all           # disable plt

# LDFLAGS +=  /lib/crt1.o           # link with c startup lib

all: test-app.hh test-app.log


%.fo : %.c
	@echo "compiling ${<} to object ${@}"
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

# read info from objs
%.log : %.fo
	readelf -a -S $^ >  $@
	$(OBJDUMP) $^ -h -j .dynstr >> $@


%.hh : %.fo
	@echo "converting object ${%.o} to c array header ${%.hh}"
	xxd -i $^ > $@

clean:
	rm *.o *.log *.hh *.fo