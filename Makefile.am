# $Id$

DISTCLEANFILES = *~ *.~*


bin_PROGRAMS = xye
xye_LDFLAGS = 


docedir = $(datadir)/@PACKAGE@
doce_DATA = ReadMe.html xye.svg NEWS ChangeLog AUTHORS

levelsdir= $(datadir)/@PACKAGE@/levels
levels_DATA = levels/5levels.xye \
levels/levels.xye \
levels/logo.xye \
levels/Mic4.xye \
levels/pxlevels.xye \
levels/microban.xsb \
levels/oldtutorials.xye \
levels/sphinx.xye \
levels/SuperJustin.xye \
levels/cracked.xye \
levels/tutorials.xye \
levels/Phipps.xye \
levels/xyester.xye \
levels/SuperJustin.xye

resdir= $(datadir)/@PACKAGE@/res
res_DATA = res/default.xml \
res/xye.png \
res/xye_luminosity.png \
res/DejaVuSans.ttf \
res/DejaVuSans-Bold.ttf \
res/fon.bmp \
res/fon_bold.bmp \
res/clean40.xml \
res/clean40.png \
res/clean40_luminosity.png \
res/detailed.xml \
res/detailed20.xml \
res/detailed30.xml \
res/detailed.png \
res/detailed20.png \
res/detailed30.png \
res/detailed_L.png \
res/detailed_L20.png \
res/detailed_L30.png \
res/detailed_icon.png \
res/default_icon.png \
res/detailed_COPYING






EXTRA_DIST1 =autogen.sh \
             clean.sh \
             build-linux.sh \
             build-win32.sh \
             cross-configure.sh \
             cross-make.sh \
src/xye.cbp \
xye.ico \
res/xye.xcf \
xye.svg \
src/resource.rc \
src/Makefile.in




xye_SOURCES = src/main.cpp src/browser.cpp \
src/font.cpp src/gen.cpp src/kye_script.cpp src/levels.cpp src/options.cpp \
src/tinyxml/xye_tinystr.cpp \
src/tinyxml/xye_tinyxml.cpp \
src/tinyxml/xye_tinyxmlerror.cpp \
src/tinyxml/xye_tinyxmlparser.cpp \
src/vxsdl.cpp src/xye.cpp src/xsb_level.cpp \
src/xye_script.cpp src/browser.h src/font.h src/gen.h src/kye_script.h src/xsb_level.h src/levels.h \
src/listbox.cpp src/listbox.h src/options.h \
src/vxsdl.h src/xye.h src/xye_script.h \
src/tinyxml/xye_tinystr.h \
src/tinyxml/xye_tinyxml.h \
src/record.cpp \
src/record.h \
src/xyedit.cpp \
src/editorsave.cpp \
src/editorload.cpp \
src/xyedit.h \
src/window.h \
src/dialogs.h \
src/dialogs.cpp \
src/window.cpp \
src/skins.cpp \
src/skins.h

EXTRA_DIST = ${EXTRA_DIST1} ${xye_SOURCES} ${res_DATA} ${levels_DATA} ${doce_DATA}

DEFS=@data_path_flag@


#xye_LDADD = @GL_LIBS@
  
