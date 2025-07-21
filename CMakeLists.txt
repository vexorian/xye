cmake_minimum_required(VERSION 3.22)

project(xye VERSION 0.12.2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_ttf REQUIRED)
#find_package(SDL3 CONFIG REQUIRED)
#find_package(SDL3_image CONFIG REQUIRED)
#find_package(SDL3_ttf CONFIG REQUIRED)

#add_compile_options($<$<CXX_COMPILER_ID:GNU,Clang>:-Wall> $<$<CXX_COMPILER_ID:GNU,Clang>:-Wextra> -Wpedantic)

add_library(tinyxml
    src/tinyxml/xye_tinystr.cpp
    src/tinyxml/xye_tinyxml.cpp
    src/tinyxml/xye_tinyxmlerror.cpp
    src/tinyxml/xye_tinyxmlparser.cpp
)

add_executable(xye
    src/browser.cpp
    src/dialogs.cpp
    src/editorload.cpp
    src/editorsave.cpp
    src/font.cpp
    src/gen.cpp
    src/kye_script.cpp
    src/levels.cpp
    src/listbox.cpp
    src/main.cpp
    src/options.cpp
    src/record.cpp
    src/skins.cpp
    src/vxsdl.cpp
    src/window.cpp
    src/xsb_level.cpp
    src/xye.cpp
    src/xyedit.cpp
    src/xye_script.cpp
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_sources(xye PRIVATE src/resource.rc)
endif()

target_link_libraries(xye
    tinyxml

    SDL::SDL
    ${SDL_IMAGE_LIBRARIES}
    ${SDL_TTF_LIBRARIES}

    #SDL3::SDL3
    #SDL3::SDL3-static
    #SDL3_image::SDL3_image
    #SDL3_image::SDL3_image-static
    #SDL3_ttf::SDL3_ttf-static
)

include(install.cmake)
