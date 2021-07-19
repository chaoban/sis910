## eCos library installation directory
#PKG_INSTALL_DIR = /cygdrive/c/Work/eCos_build/sis910_install
PKG_INSTALL_DIR=$$(PKG_INSTALL_DIR)

## This sets the compiler to R16.
XCC = sh-elf-gcc

##
## Build flag descriptions.
##
## Compiler flags
##
##   -g						: Produce debugging information.
##   -Wall					: Enable all preprocessor warnings.
##   -I						: Add directory to head of list to be searched for header files.
##   -ffunction-sections	: Place each function into its own section.
##   -fdata-sections		: Discard unused virtual functions.
##
## Linker flags
##
##   -nostartfiles			: Do not use the standard system startup files when linking.
##							  The standard system libraries are used normally, unless
##							  -nostdlib or -nodefaultlibs is used.
##   -L						: Add DIRECTORY to library search path.
##   -Wl					: Pass comma-separated <options> on to the linker.
##   --gc-sections			: Remove unused sections (on some targets).
##   -Map					: Write a map file.
##   -T						: Read linker script.
##   -nostdlib				: Do not use the standard system startup files or
##							  libraries when linking. No startup files and only
##							  the libraries you specify will be passed to the linker.
##

## Build flags.
CFLAGS	= -g -Wall -I$(PKG_INSTALL_DIR)/include -ffunction-sections -fdata-sections
LDFLAGS	= -nostartfiles -L$(PKG_INSTALL_DIR)/lib -Wl,--gc-sections -Wl,--Map -Wl,ms_test.map
LIBS	= -Ttarget.ld -nostdlib
LD		= $(XCC)


## Build rules.
all: ms_test

#printt.o: printt.c
#	$(XCC) -c -o $*.o $(CFLAGS) $<
    
ms_test.o: ms_test.c
	$(XCC) -c -o $*.o $(CFLAGS) $<

ms_test: ms_test.o
	$(LD) $(LDFLAGS) -o $@ $@.o $(LIBS)

clean:
	-rm -f ms_test ms_test.o ms_test.map
