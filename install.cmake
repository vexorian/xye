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
    res/xmaskye_jr.xml
    res/XMASKye.png
    res/XMASKye_luminosity.png
    res/xmaskye_jr32.xml
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

set(CPACK_GENERATOR "DEB" "RPM" "TBZ2")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
set(CPACK_RPM_PACKAGE_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})

#set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
#set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

#set(CPACK_PACKAGE_NAME "xye")
#set(CPACK_PACKAGE_VERSION ${CMAKE_PROJECT_VERSION})
#set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Xye a puzzle game with very different levels")

set(CPACK_PACKAGE_CONTACT "vexorian@gmail.com")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/vexorian/xye")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
#set(CPACK_PACKAGE_VENDOR "My Company")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
#set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_STRIP_FILES YES)
#set(CPACK_RPM_COMPONENT_INSTALL ON)
#set(CPACK_DEB_COMPONENT_INSTALL ON)

include(CPack)

#configure_file(${PROJECT_SOURCE_DIR}/XyeCPackOptions.cmake.in
#               ${PROJECT_BINARY_DIR}/XyeCPackOptions.cmake @ONLY)
#set(CPACK_PROJECT_CONFIG_FILE ${PROJECT_BINARY_DIR}/XyeCPackOptions.cmake)
