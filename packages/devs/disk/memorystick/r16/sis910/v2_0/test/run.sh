#!/bin/bash

make clean
make PKG_INSTALL_DIR=/cygdrive/c/Work/eCos_build/sis910_install
sh-elf-objcopy -O binary ms_test ms_test.bin
cp ms_test.bin //192.168.94.229/SW_use/Sidney/
