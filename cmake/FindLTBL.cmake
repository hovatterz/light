# - Find LTBL
# Find the LTBL includes and library
#
#   LTBL_INCLUDE_DIR - LBTL include directory.
#   LTBL_LIBRARY     - LBTL library.
#
find_path(LTBL_INCLUDE_DIR
    LTBL/LightSystem.h
    PATH_SUFFIXES include
    PATHS /usr /usr/local /opt/local)

find_library(LTBL_LIBRARY
    ltbl
    PATH_SUFFIXES lib lib64
    PATHS /usr /usr/local /opt/local)

if(NOT LTBL_INCLUDE_DIR OR NOT LTBL_LIBRARY)
    message(FATAL_ERROR "LTBL not found.")
else()
    message("LTBL found: ${LTBL_INCLUDE_DIR}")
endif()
