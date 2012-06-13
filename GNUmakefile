################################################################################
# Makefile: GNUMakefile to compile project.
#
# Notes:
#		* Best viewed with tabstop=3
#     * -fno-strict-aliasing is *only* present in cflags
#			because of MDM_MFREE macro.
#
# BUGS:
#
# Author: Marcelo Gornstein <marcelog@netlabs.com.ar>
################################################################################

VERSION = 0.1.1
################################################################################
# Do not touch anything below here.
################################################################################
.SUFFIXES =
.SUFFIXES = .c .o .a
DOXYGEN=/usr/bin/doxygen
OUTPUTDIR += ./build
SRCDIR += .
TARGET += x86_64-pc-linux-gnu
OBJDIR += ${OUTPUTDIR}/${TARGET}/obj
BINDIR += ${OUTPUTDIR}/${TARGET}/bin
LIBDIR += ${OUTPUTDIR}/${TARGET}/lib
DOCDIR += ${OUTPUTDIR}/doc
PKGDIR += ${OUTPUTDIR}/${TARGET}/pkg
CFGPKGDIR += ${OUTPUTDIR}
CFGDIR += ./conf
CLIENTSDIR += ./clients
SHELL = /bin/bash
NAME = MDM
BUILDROOT += /usr
CC = ${TARGET}-gcc
AR = ${TARGET}-ar
LD = ${TARGET}-ld
CFLAGS = \
	-g -Wall -pedantic -ansi -D_BSD_SOURCE -D_GNU_SOURCE -std=c99 -Wmissing-prototypes \
	-pipe -O2 -fPIC -pedantic-errors -Wfatal-errors -Wunused -Wuninitialized \
	-Wformat -Wno-format-zero-length -Wno-format-extra-args \
	-Wformat-nonliteral -Wformat-security -Wpointer-arith -Wcast-align \
	-Wcast-qual -Wbad-function-cast -Wshadow -fno-strict-aliasing \
	-I${BUILDROOT}/include -I${BUILDROOT}/include/libxml2 -I./include -L. -L${BUILDROOT}/lib \
	-I${BUILDROOT}/libiconv -I${BUILDROOT}/libtelnet -I${BUILDROOT}/libssh2

STRIP = echo
LIBNAME = ${LIBDIR}/lib${NAME}.a
realLibs += -ltelnet -lssh2 -lm -lxml2 ${LIBS}
BINARIES = \
	${BINDIR}/mdm_test_driver_dummy \
	${BINDIR}/mdm_test_driver_rawtcp \
	${BINDIR}/mdm_test_driver_telnet \
	${BINDIR}/mdm_test_driver_serial \
	${BINDIR}/mdm_test_driver_ssh2 \
	${BINDIR}/mdm_device_types_list \
	${BINDIR}/mdm_connection_types_list \
	${BINDIR}/mdm_test_device_dslam_alcatel_73xx \
	${BINDIR}/mdm_test_device_dslam_zte_8426 \
	${BINDIR}/mdm_test_device_dslam_zte_9xxx \
	${BINDIR}/mdm_test_device_dslam_huawei_ma5600 \
	${BINDIR}/mdm_test_device_dslam_siemens_hix5300 \
	${BINDIR}/mdm_server

LIBRARY = \
	${OBJDIR}/mdm_parser_dslam_alcatel_73xx.o \
	${OBJDIR}/mdm_parser_dslam_zte_9xxx.o \
	${OBJDIR}/mdm_parser_dslam_huawei_ma5600.o \
	${OBJDIR}/mdm_parser_dslam_zte_8426.o \
	${OBJDIR}/mdm_parser_dslam_siemens_hix5300.o \
	${OBJDIR}/mdm_device_dslam_zte_8426.o \
	${OBJDIR}/mdm_device_dslam_huawei_ma5600.o \
	${OBJDIR}/mdm_device_dslam_siemens_hix5300.o \
	${OBJDIR}/mdm_device_dslam_zte_9xxx.o \
	${OBJDIR}/mdm_device_dslam_alcatel_73xx.o \
	${OBJDIR}/mdm.o \
	${OBJDIR}/mdm_device.o \
	${OBJDIR}/mdm_config.o \
	${OBJDIR}/mdm_mm.o \
	${OBJDIR}/mdm_log.o \
	${OBJDIR}/mdm_connection.o \
	${OBJDIR}/mdm_driver_dummy.o \
	${OBJDIR}/mdm_driver_telnet.o \
	${OBJDIR}/mdm_driver_ssh2.o \
	${OBJDIR}/mdm_driver_rawtcp_server.o \
	${OBJDIR}/mdm_driver_serial.o \
	${OBJDIR}/mdm_driver_rawtcp.o

all: prepare library binaries package
library: ${LIBNAME}
binaries: library ${BINARIES}

${LIBNAME}: ${LIBRARY}
	${AR} rcs $@ $^

doxygen:
	cd ${SRCDIR} && ${DOXYGEN} doxygen.cfg && cd -

package:
	tar jcf ${PKGDIR}/${TARGET}.tar.bz2 ${BINDIR}

packageconf:
	tar jcf ${CFGPKGDIR}/config-files.tar.bz2 ${CFGDIR}

packageclients:
	tar jcf ${CFGPKGDIR}/clients-example.tar.bz2 ${CLIENTSDIR}

prepare:
	mkdir -p ${BINDIR}
	mkdir -p ${OBJDIR}
	mkdir -p ${LIBDIR}
	mkdir -p ${DOCDIR}
	mkdir -p ${PKGDIR}

clean:
	rm -rf ${BINDIR}
	rm -rf ${OBJDIR}
	rm -rf ${LIBDIR}
	rm -rf ${PKGDIR}

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	${CC} -c ${CFLAGS} -o $@ $<

$(BINDIR)/%: $(SRCDIR)/%.c
	${CC} ${CFLAGS} $^ -o $@ ${realLibs} ${LIBNAME}
#################################################################################
## Makefile ENDS.
#################################################################################
