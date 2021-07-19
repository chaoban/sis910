# uncertain
WORK_DIR	= /work_tmp/ecos
KERNEL_DIR	= $(WORK_DIR)/i386/madmp30930_install
KERNEL_INC	= $(KERNEL_DIR)/include

include $(KERNEL_INC)/pkgconf/ecos.mak

SRC_C	= mp3player.c	\

# tools
RM	?= del /Q /S
GCC	= i386-elf-gcc
GLD	= $(GCC)
GDB	= $(ECOS_COMMAND_PREFIX)gdb
OBJCOPY	= i386-elf-objcopy

# flags
CFLAGS	= -I$(KERNEL_INC)
DFLAGS	= -DAUD_DEBUG
LDFLAGS	= -nostartfiles -L$(KERNEL_DIR)/lib -Ttarget.ld

# rules
%.o: %.c
	@echo Compiling $< ...
	@$(GCC) -c -o $@ $(CFLAGS) $(DFLAGS) $<

# PHONY
.PHONY: mp3player clean objcopy gdb test

gdb:
	@echo i386-elf-gdb mp3player.exe
	@i386-elf-gdb.exe mp3player.exe

objcopy:
	@echo Make srec file ...
	@$(OBJCOPY) -O srec mp3player.exe mp3player.srec

mp3player: $(subst .c,.o,$(SRC_C))
	@echo Linking $^ ...
	@$(GCC) $^ $(LDFLAGS) $(ECOS_GLOBAL_LDFLAGS) -o $@.exe

test: fatfs1.o
	@$(GCC) $< $(LDFLAGS) $(ECOS_GLOBAL_LDFLAGS) -o $@.exe

clean:
	@echo Deleting tmp files: *~
	@echo Deleting objects: *.o
	@echo Deleting exe files: *.exe
	@echo Deleting srec files: *.srec
	@$(RM) *~ *.o *.exe *.srec

