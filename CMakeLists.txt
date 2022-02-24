cmake_minimum_required (VERSION 3.5.1)

project(font2bitmap)

# "For this is how God loved the world:
# he gave his only Son, so that everyone
# who believes in him may not perish
# but may have eternal life."
#
# John 3:16

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

ENABLE_LANGUAGE(CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# avoid base libs overwrite
option(ARIBEIRO_SKIP_INSTALL_CORE ON)
option(ARIBEIRO_SKIP_INSTALL_PLATFORM ON)
option(ARIBEIRO_SKIP_INSTALL_DATA ON)

include(cmake/include.cmake)
include(cmake/libs/libaRibeiroCore.cmake)
include(cmake/libs/libaRibeiroPlatform.cmake)
include(cmake/libs/libaRibeiroData.cmake)

include(cmake/libs/libconvertutf.cmake)
include(cmake/libs/libfreetype.cmake)
include(cmake/libs/aRibeiroWrappers.cmake)
tool_include_lib(aRibeiroWrappers ft2-wrapper)

FILE( GLOB HEADERS src/*.h )
FILE( GLOB SRC src/*.cpp )

if(WIN32)
    set( ADITIONAL_SRC src/win32/getopt.c src/win32/getopt.h )
    set( ADITIONAL_DIR src/win32 )
    
    SOURCE_GROUP(win32 FILES win32/getopt.c win32/getopt.h)
ENDIF()

include_directories(${ADITIONAL_DIR})

add_executable(${PROJECT_NAME}
        ${HEADERS}
        ${SRC}
        ${ADITIONAL_SRC}
)

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "TOOLS")

# Linking libraries
target_link_libraries( 
    ${PROJECT_NAME}
    convertutf freetype ft2-wrapper
    aRibeiroCore aRibeiroPlatform aRibeiroData
)

copy_file_after_build(
    ${PROJECT_NAME} 
    resources/charset.utf8
    resources/mailrays.ttf
    resources/CreateFontWin32.bat
    resources/goudy_bookletter_1911.otf
    resources/Roboto-Light.ttf
    resources/Roboto-Regular.ttf
)


if( NOT MSVC )
    # Install setup
    install(
        TARGETS ${PROJECT_NAME} 
        RUNTIME DESTINATION bin
    )
endif()
