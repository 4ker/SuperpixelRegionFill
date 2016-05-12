project( SuperpixelRegionFill )
cmake_minimum_required( VERSION 2.8.3 )

set( CMAKE_COLOR_MAKEFILE ON )
set_property( GLOBAL PROPERTY USE_FOLDERS ON )

# Allow advanced users to generate Makefiles printing detailed commands
mark_as_advanced( CMAKE_VERBOSE_MAKEFILE )

# Path to additional CMake modules
set( CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules ${CMAKE_MODULE_PATH} )
set( CMAKE_DEBUG_POSTFIX "d" )

string( TOUPPER ${PROJECT_NAME} PROJECT_NAME_CAPITALIZED )
add_definitions( -DUNICODE )

# large address aware option setting
if( ${MINGW} )
    set( LARGEADDRESSAWARE "--large-address-aware" )
else( ${MINGW} )
    if ( WIN32 )
        set( LARGEADDRESSAWARE "/LARGEADDRESSAWARE" )
    else ( WIN32 )
        set( LARGEADDRESSAWARE " " )
    endif ( WIN32 )
endif( ${MINGW} )

if ( WIN32)
    SET( APP_OPT "WIN32" )
else ( WIN32 )
    if ( APPLE )
        #SET( APP_OPT "MACOSX_BUNDLE" )
        SET( APP_OPT "EXCLUDE_FROM_ALL" )
    else ( APPLE )
        SET( APP_OPT " " )
    endif ( APPLE )
endif ( WIN32 )

# OpenCV 
find_package( OpenCV REQUIRED )

# Qt4 
find_package( Qt4 REQUIRED )

# include them all
include( ${QT_USE_FILE} )
include_directories( ${OPENCV_INCLUDE_DIR} )
include_directories( ${OpenCV_INCLUDE_DIRS} )

message( STATUS "OpenCV library status:" )
message( STATUS "    version: ${OpenCV_VERSION}" )
message( STATUS "    libraries: ${OpenCV_LIBS}" )
message( STATUS "    include path: ${OpenCV_INCLUDE_DIRS}" )

include_directories( ${CMAKE_SOURCE_DIR} )
include_directories( ${CMAKE_BINARY_DIR} )
include_directories( ${CMAKE_SOURCE_DIR}/src/LoadImage )
include_directories( ${CMAKE_BINARY_DIR}/src/LoadImage )
include_directories( ${CMAKE_SOURCE_DIR}/src/Seeds )
include_directories( ${CMAKE_BINARY_DIR}/src/Seeds )
include_directories( ${CMAKE_SOURCE_DIR}/src/Utils )
include_directories( ${CMAKE_BINARY_DIR}/src/Utils )

link_directories( ${CMAKE_BINARY_DIR}/ )

# Setup output directories
set( LIBRARY_OUTPUT_PATH 
    ${PROJECT_BINARY_DIR}/bin
    CACHE
    PATH
    "Single directory for all libraries" )
    
set( EXECUTABLE_OUTPUT_PATH 
    ${PROJECT_BINARY_DIR}/bin
    CACHE
    PATH
    "Single directory for all executables" )

file( GLOB BT_RCS_RAW *.qrc )

configure_file( 
    "${PROJECT_SOURCE_DIR}/Configs.h.in"
    "${PROJECT_BINARY_DIR}/Configs.h" )

add_subdirectory( src )
add_subdirectory( examples )
