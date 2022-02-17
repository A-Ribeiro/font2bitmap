set( LIB_OPENGL UsingFindPackage CACHE STRING "Choose the Library Source." )
set_property(CACHE LIB_OPENGL PROPERTY STRINGS None UsingFindPackage)

if(LIB_OPENGL STREQUAL UsingFindPackage)

    if (NOT TARGET LinkOpenGL)
        message(STATUS "[LIB_OPENGL] using system lib.")

        if (ARIBEIRO_RPI)
            set(OPENGL_LIBRARIES ${GLES_LIBRARY})
        else()
            find_package(OpenGL REQUIRED QUIET)
            include_directories(${OPENGL_INCLUDE_DIR} PARENT_SCOPE)
        endif()
        
        add_library(LinkOpenGL OBJECT ${OPENGL_LIBRARIES})
        target_link_libraries(LinkOpenGL ${OPENGL_LIBRARIES})
        
        # set the target's folder (for IDEs that support it, e.g. Visual Studio)
        set_target_properties(LinkOpenGL PROPERTIES FOLDER "LIBS")
    endif()

else()
    message( FATAL_ERROR "You need to specify the lib source." )
endif()
