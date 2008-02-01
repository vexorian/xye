#!/bin/sh
# $Id$

if !(./configure) then exit 1; fi
make clean
make
