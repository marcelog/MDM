#!/bin/bash
gmake SRCDIR=./src OUTPUTDIR=./obj-linux TARGET=x86_64-redhat-linux BUILDROOT=/usr LIBS="-lpthread" clean all

