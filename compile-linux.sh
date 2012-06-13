#!/bin/bash
gmake SRCDIR=./src TARGET=x86_64-redhat-linux BUILDROOT=/usr LIBS="-lpthread" clean all

