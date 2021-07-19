# uncertain
WORK_DIR	= /cygdrive/d/BOOKs/MP3_Reference/cygwin/wordir
KERNEL_DIR	= $(WORK_DIR)/sis910-ftp/net_install
KERNEL_INC	= $(KERNEL_DIR)/include

include $(KERNEL_INC)/pkgconf/ecos.mak

SRC_C	= mp3player.c	\

# tools
RM	= rm -rf
GCC	= $(ECOS_COMMAND_PREFIX)gcc
GLD	= $(GCC)
GDB	= $(ECOS_COMMAND_PREFIX)gdb
OBJCOPY	= $(ECOS_COMMAND_PREFIX)objcopy

# flags
CFLAGS	= -I$(KERNEL_INC)
DFLAGS	= -DAUD_DEBUG
LDFLAGS	= -nostartfiles -L$(KERNEL_DIR)/lib -Ttarget.ld

# rules
%.o: %.c
	@echo Compiling $< ...
	@$(GCC) -c -o $@ $(CFLAGS) $(DFLAGS) $(ECOS_GLOBAL_CFLAGS) $<

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

test: flush.o
	@$(GCC) $< $(LDFLAGS) $(ECOS_GLOBAL_LDFLAGS) -o $@.exe

clean:
	@echo Deleting tmp files: *~
	@echo Deleting objects: *.o
	@echo Deleting exe files: *.exe
	@echo Deleting srec files: *.srec
	@$(RM) *~ *.o *.exe *.srec

