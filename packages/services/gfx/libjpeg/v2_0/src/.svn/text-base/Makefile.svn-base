##############################################################################
# Microwindows template Makefile
# Copyright (c) 2000 Martin Jolicoeur, Greg Haerr
# Portions Copyright (c) 2002 by Koninklijke Philips Electronics N.V.
##############################################################################

ifndef MW_DIR_SRC
MW_DIR_SRC := $(CURDIR)/../..
endif
MW_DIR_RELATIVE := engine/jpg6.1/

include $(MW_DIR_SRC)/Path.rules
include $(CONFIG)

############################# targets section ################################

# If you want to create a library with the objects files, define the name here

LIBNAME = libmwjpeg.a

OBJS := \
	$(MW_DIR_OBJ)/engine/jpg6.1/jcomapi.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jdapimin.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jdapistd.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jdatasrc.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jdcoefct.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jdcolor.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jddctmgr.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jdhuff.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jdinput.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jdmainct.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jdmarker.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jdmaster.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jdmerge.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jdphuff.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jdpostct.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jdsample.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jerror.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jidctflt.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jidctfst.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jidctint.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jidctred.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jmemmgr.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jmemnobs.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jquant1.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jquant2.o \
	$(MW_DIR_OBJ)/engine/jpg6.1/jdsishwjpec.o\
	$(MW_DIR_OBJ)/engine/jpg6.1/jutils.o


######################### Makefile.rules section #############################

include $(MW_DIR_SRC)/Makefile.rules








