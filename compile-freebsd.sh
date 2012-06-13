#!/bin/bash
crossfreebsd=/home/marcelog/cross-freebsd

# Build FreeBSD
export PATH=${crossfreebsd}/bin:/bin:/sbin:/usr/bin:/usr/sbin
export LD_LIBRARY_PATH=${crossfreebsd}/lib
gmake SRCDIR=./src TARGET=x86_64-pc-freebsd7 BUILDROOT=${crossfreebsd} LIBS="-lz -liconv -lgcrypt -lgpg-error -lpthread" clean all

