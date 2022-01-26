cmake_minimum_required(VERSION 3.5)

project(TaggedObjectRemovalApp VERSION 0.1 LANGUAGES C CXX)

# =============== CONFIG ===============
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ================ Find QT ================
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
set(QT_TARGET Qt${QT_VERSION_MAJOR})

message("-- QT version: " ${QT_TARGET} )

find_package(${QT_TARGET} COMPONENTS Widgets REQUIRED)

# ================ Find CV2 =================
set(OpenCV_DIR ./opencv)
find_package(OpenCV REQUIRED)

# Windows Only
file(GLOB OPENCV_FILES_TO_COPY "${_OpenCV_LIB_PATH}/opencv_world*.dll")
# ================ CUSTOM ==================

# Usage:
# qt_add_library(${TARGET} 
#     SOURCES
#        source.cpp
#     UI
#        source.ui
# )
macro(qt_add_library target)
    cmake_parse_arguments(PARSED # prefix
        "" # bools 
        "" # single values
        "SOURCES;UI"  # multi values
        ${ARGN})
    
    if (PARSED_UI)
        qt_wrap_ui(PARSED_SOURCES ${PARSED_UI})
    endif()

    add_library(${target} ${PARSED_SOURCES})
endmacro()

# ================ Subfolders ===============
add_subdirectory(src)
add_subdirectory(submodules)


