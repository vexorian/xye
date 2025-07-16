#!/bin/sh
# $Id$

PROGNAME=xye

if !(./cross-configure.sh) then exit 1; fi
./cross-make.sh clean
./cross-make.sh
if test $? -ne 0; then exit 1; fi
test -w src/$PROGNAME && chmod -x src/$PROGNAME
test -w src/$PROGNAME && mv src/$PROGNAME src/$PROGNAME.exe
