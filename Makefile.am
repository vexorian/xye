# $Id$

DISTCLEANFILES = *~ *.~*


bin_PROGRAMS = xye
#in case of debugging comment next line
xye_LDFLAGS = 


docedir = $(doc_dir)/@PACKAGE@
doce_DATA = README GAMEINTRO.txt xye.svg NEWS COPYING AUTHORS INSTALL

levelsdir= $(datadir)/@PACKAGE@/levels
levels_DATA = levels/5levels.xye \
levels/kye.xye \
levels/levels.xye \
levels/logo.xye \
levels/Mic4.xye \
levels/pxlevels.xye \
levels/microban.xsb \
levels/oldtutorials.xye \
levels/sphinx.xye \
levels/cracked.xye \
levels/tutorials.xye \
levels/xyester.xye

resdir= $(datadir)/@PACKAGE@/res
res_DATA = res/default.xml \
res/classic.xml \
res/xye.png \
res/kye.png \
res/DejaVuSans.ttf \
res/DejaVuSans-Bold.ttf \
res/fon.bmp \
res/fon_bold.bmp





EXTRA_DIST = readme.txt \
             autogen.sh \
             clean.sh \
             build-linux.sh \
             build-win32.sh \
             cross-configure.sh \
             cross-make.sh \
src/xye.cbp \
xye.ico \
res/kye.xcf \
res/xye.xcf \
xye.svg \
src/resource.rc





xye_SOURCES = src/main.cpp src/browser.cpp \
src/font.cpp src/gen.cpp src/kye_script.cpp src/levels.cpp src/list.cpp src/options.cpp \
src/tinyxml/xye_tinystr.cpp \
src/tinyxml/xye_tinyxml.cpp \
src/tinyxml/xye_tinyxmlerror.cpp \
src/tinyxml/xye_tinyxmlparser.cpp \
src/vxsdl.cpp src/xye.cpp src/xsb_level.cpp \
src/xye_script.cpp src/browser.h src/font.h src/gen.h src/kye_script.h src/xsb_level.h src/levels.h \
src/list.h src/options.h \
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
src/command.cpp \
src/command.h \
src/window.cpp

DEFS=@data_path_flag@


#xye_LDADD = @GL_LIBS@
  
