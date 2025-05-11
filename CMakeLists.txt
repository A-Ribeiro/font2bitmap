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

# reset global variables
unset (CMAKE_MODULE_PATH CACHE)
unset (CMAKE_PREFIX_PATH CACHE)

add_subdirectory(libs/InteractiveToolkit "${CMAKE_BINARY_DIR}/lib/InteractiveToolkit")
add_subdirectory(libs/InteractiveToolkit-Extension "${CMAKE_BINARY_DIR}/lib/InteractiveToolkit-Extension")

# show info about the lib parameters
find_package(InteractiveToolkit REQUIRED)

if (NOT TARGET ft2-wrapper)
    add_subdirectory(libs/InteractiveToolkit-Extension/wrappers/ft2-wrapper "${CMAKE_BINARY_DIR}/lib/ft2-wrapper")
endif()

FILE( GLOB HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" src/*.h)
FILE( GLOB SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" src/*.cpp)

if(WIN32)
    set( ADITIONAL_SRC src/win32/getopt.c src/win32/getopt.h )
    set( ADITIONAL_DIR src/win32 )    
    # SOURCE_GROUP(win32 FILES win32/getopt.c win32/getopt.h)
endif()

tool_define_source_group(
    ${HEADERS}
    ${SRC}
    ${ADITIONAL_SRC}
)

add_executable( ${PROJECT_NAME}
    ${HEADERS}
    ${SRC}
    ${ADITIONAL_SRC}
)

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "TOOLS")

target_include_directories(${PROJECT_NAME} PUBLIC ${ADITIONAL_DIR})
# Linking libraries
target_link_libraries( ${PROJECT_NAME} 
    PUBLIC
    InteractiveToolkit-Extension
    ft2-wrapper
)

tool_copy_directory_after_build( ${PROJECT_NAME} resources )

tool_copy_file_after_build(
    ${PROJECT_NAME} 
    resources/charset.utf8
    resources/mailrays.ttf
    resources/CreateFontLinux.sh
    resources/CreateFontWin32.bat
    resources/goudy_bookletter_1911.otf
    resources/Roboto-Light.ttf
    resources/Roboto-Regular.ttf
    resources/custom_chars.ini
    resources/R_generate_with_ini.bat
)

if( NOT MSVC )
    install(
        TARGETS ${PROJECT_NAME} 
        RUNTIME DESTINATION bin
    )
endif()
