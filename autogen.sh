#!/bin/sh
# $Id$

# Remove any previously created cache files
test -w config.cache && rm config.cache
test -w config.cross.cache && rm config.cross.cache

# Regenerate configuration files
echo " ========aclocal============="
aclocal
echo " ======== / aclocal ============="
echo " ========Automake============="
automake --add-missing
echo " ======== / Automake============="
echo " ========autoconf============="
autoconf
echo " ======== / autoconf============="

# Run configure for this platform
#./configure $*
echo "Now you are ready to run ./configure"
