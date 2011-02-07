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
# TODO:
#		* Better makefile.
#
# Author: Marcelo Gornstein <marcelog@gmail.com>
################################################################################

VERSION = 0.1.1
################################################################################
# Do not touch anything below here.
################################################################################
NAME = MDM
CC = gcc
AR = ar
DOXYGEN = doxygen
CFLAGS = \
	-g -Wall -pedantic -ansi -D_BSD_SOURCE -D_GNU_SOURCE -std=c99 -Wmissing-prototypes \
	-pipe -O2 -fPIC -pedantic-errors -Wfatal-errors -Wunused -Wuninitialized \
	-Wformat -Wno-format-zero-length -Wno-format-extra-args \
	-Wformat-nonliteral -Wformat-security -Wpointer-arith -Wcast-align \
	-Wcast-qual -Wbad-function-cast -Wshadow -fno-strict-aliasing \
	-pthread -I./include -L./lib -I/usr/include -L/usr/lib \
	-I/usr/include/libxml2 -I/usr/include/libiconv

STRIP = echo
LD = ld
LIBNAME = lib${NAME}
LIBS = -ltelnet -lcurl -lssh2 -lm -lrt -lxml2
all: clean so binaries #doxygen

binaries: \
	mdm_test_driver_dummy \
	mdm_test_driver_rawtcp \
	mdm_test_driver_telnet \
	mdm_test_driver_serial \
	mdm_test_driver_ssh2 \
	mdm_device_types_list \
	mdm_connection_types_list \
	mdm_test_device_dslam_alcatel_73xx \
	mdm_test_device_dslam_zte_8426 \
	mdm_test_device_dslam_zte_9xxx \
	mdm_test_device_dslam_huawei_ma5600 \
	mdm_server \

mdm_test_driver_dummy:
	${CC} ${CFLAGS} src/mdm_test_driver_dummy.c -o bin/mdm_test_driver_dummy -l${NAME} ${LIBS}
	${STRIP} bin/mdm_test_driver_dummy

mdm_test_driver_rawtcp:
	${CC} ${CFLAGS} src/mdm_test_driver_rawtcp.c -o bin/mdm_test_driver_rawtcp -l${NAME} ${LIBS}
	${STRIP} bin/mdm_test_driver_rawtcp

mdm_test_driver_telnet:
	${CC} ${CFLAGS} src/mdm_test_driver_telnet.c -o bin/mdm_test_driver_telnet -l${NAME} ${LIBS}
	${STRIP} bin/mdm_test_driver_telnet

mdm_test_driver_serial:
	${CC} ${CFLAGS} src/mdm_test_driver_serial.c -o bin/mdm_test_driver_serial -l${NAME} ${LIBS}
	${STRIP} bin/mdm_test_driver_serial

mdm_test_driver_ssh2:
	${CC} ${CFLAGS} src/mdm_test_driver_ssh2.c -o bin/mdm_test_driver_ssh2 -l${NAME} ${LIBS}
	${STRIP} bin/mdm_test_driver_ssh2

mdm_device_types_list:
	${CC} ${CFLAGS} src/mdm_device_types_list.c -o bin/mdm_device_types_list -l${NAME} ${LIBS}
	${STRIP} bin/mdm_device_types_list

mdm_connection_types_list:
	${CC} ${CFLAGS} src/mdm_connection_types_list.c -o bin/mdm_connection_types_list -l${NAME} ${LIBS}
	${STRIP} bin/mdm_connection_types_list

mdm_test_device_dslam_alcatel_73xx:
	${CC} ${CFLAGS} src/mdm_test_device_dslam_alcatel_73xx.c -o bin/mdm_test_device_dslam_alcatel_73xx -l${NAME} ${LIBS}
	${STRIP} bin/mdm_test_device_dslam_alcatel_73xx

mdm_test_device_dslam_zte_8426:
	${CC} ${CFLAGS} src/mdm_test_device_dslam_zte_8426.c -o bin/mdm_test_device_dslam_zte_8426 -l${NAME} ${LIBS}
	${STRIP} bin/mdm_test_device_dslam_zte_8426

mdm_test_device_dslam_zte_9xxx:
	${CC} ${CFLAGS} src/mdm_test_device_dslam_zte_9xxx.c -o bin/mdm_test_device_dslam_zte_9xxx -l${NAME} ${LIBS}
	${STRIP} bin/mdm_test_device_dslam_zte_9xxx

mdm_test_device_dslam_huawei_ma5600:
	${CC} ${CFLAGS} src/mdm_test_device_dslam_huawei_ma5600.c -o bin/mdm_test_device_dslam_huawei_ma5600 -l${NAME} ${LIBS}
	${STRIP} bin/mdm_test_device_dslam_huawei_ma5600

mdm_server:
	${CC} ${CFLAGS} src/mdm_server.c -o bin/mdm_server -l${NAME} ${LIBS} -pthread
	${STRIP} bin/mdm_server

so:
	${CC} ${CFLAGS} -c src/mdm.c -o obj/mdm.o
	${CC} ${CFLAGS} -c src/mdm_config.c -o obj/mdm_config.o
	${CC} ${CFLAGS} -c src/mdm_log.c -o obj/mdm_log.o
	${CC} ${CFLAGS} -c src/mdm_mm.c -o obj/mdm_mm.o
	${CC} ${CFLAGS} -c src/mdm_connection.c -o obj/mdm_connection.o
	${CC} ${CFLAGS} -c src/mdm_device.c -o obj/mdm_device.o
	${CC} ${CFLAGS} -c src/mdm_device_dslam_alcatel_73xx.c -o obj/mdm_device_dslam_alcatel_73xx.o
	${CC} ${CFLAGS} -c src/mdm_parser_dslam_alcatel_73xx.c -o obj/mdm_parser_dslam_alcatel_73xx.o
	${CC} ${CFLAGS} -c src/mdm_device_dslam_huawei_ma5600.c -o obj/mdm_device_dslam_huawei_ma5600.o
	${CC} ${CFLAGS} -c src/mdm_parser_dslam_huawei_ma5600.c -o obj/mdm_parser_dslam_huawei_ma5600.o
	${CC} ${CFLAGS} -c src/mdm_device_dslam_zte_8426.c -o obj/mdm_device_dslam_zte_8426.o
	${CC} ${CFLAGS} -c src/mdm_parser_dslam_zte_8426.c -o obj/mdm_parser_dslam_zte_8426.o
	${CC} ${CFLAGS} -c src/mdm_device_dslam_zte_9xxx.c -o obj/mdm_device_dslam_zte_9xxx.o
	${CC} ${CFLAGS} -c src/mdm_parser_dslam_zte_9xxx.c -o obj/mdm_parser_dslam_zte_9xxx.o
	${CC} ${CFLAGS} -c src/mdm_driver_dummy.c -o obj/mdm_driver_dummy.o
	${CC} ${CFLAGS} -c src/mdm_driver_rawtcp.c -o obj/mdm_driver_rawtcp.o
	${CC} ${CFLAGS} -c src/mdm_driver_rawtcp_server.c -o obj/mdm_driver_rawtcp_server.o
	${CC} ${CFLAGS} -c src/mdm_driver_telnet.c -o obj/mdm_driver_telnet.o
	${CC} ${CFLAGS} -c src/mdm_driver_serial.c -o obj/mdm_driver_serial.o
	${CC} ${CFLAGS} -c src/mdm_driver_ssh2.c -o obj/mdm_driver_ssh2.o
	${AR} rcs lib/${LIBNAME}.a obj/*.o
#	${CC} ${CFLAGS} -fpic -fPIC -static obj/*.o -o lib/${LIBNAME}-${VERSION}.a
#	${STRIP} lib/${LIBNAME}-${VERSION}.so
#	ln -s ${LIBNAME}-${VERSION}.so lib/${LIBNAME}.so

clean:
	rm -rf obj/*
	rm -rf lib/*
	rm -rf bin/*
	rm -rf doc/*

doxygen:
	mkdir -p build/html/doxygen
	${DOXYGEN} doxygen.cfg

################################################################################
# Makefile ENDS.
################################################################################
