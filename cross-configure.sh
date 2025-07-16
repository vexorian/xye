#!/bin/sh

PATH=/opt/SDL-1.2.13/bin:$PATH ./configure --target=i586-mingw32msvc --host=i586-mingw32msvc --build=i586-linux $*

