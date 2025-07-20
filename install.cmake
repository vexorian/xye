# install.cmake

install(TARGETS xye DESTINATION bin)

install(
FILES
    AUTHORS
    ChangeLog
    NEWS
    #ReadMe.html
    README.md
DESTINATION share/xye
)

install(DIRECTORY levels DESTINATION share/xye)

install(
FILES
    res/clean40_luminosity.png
    res/clean40.png
    res/clean40.xml
    res/default_icon.png
    res/default.xml
    res/DejaVuSans-Bold.ttf
    res/DejaVuSans.ttf
    res/detailed20.png
    res/detailed20.xml
    res/detailed30.png
    res/detailed30.xml
    res/detailed_COPYING
    res/detailed_icon.png
    res/detailed_L20.png
    res/detailed_L30.png
    res/detailed_L.png
    res/detailed.png
    res/detailed.xml
    res/classic.xml
    res/kye.png
    res/kye_luminosity.png
    res/classic32.xml
    res/kye32.png
    res/kye32_luminosity.png
    res/xmaskye jr.xml
    res/XMASKye.png
    res/XMASKye_luminosity.png
    res/xmaskye jr32.xml
    res/XMASKye32.png
    res/XMASKye_luminosity32.png
    res/fon.bmp
    res/fon_bold.bmp
    res/xye_luminosity.png
    res/xye.png
    #res/xye.xcf
DESTINATION share/xye/res
)

install(
FILES
    #pixmaps/xye.ico
    pixmaps/xye.png
    #pixmaps/xye.svg
    #pixmaps/xye.xpm
DESTINATION share/xye/pixmaps
)

install(
FILES
    xye.desktop
DESTINATION share/application
)
