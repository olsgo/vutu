# FindLoris.cmake
# Locate Loris library
# This module defines
# LORIS_FOUND, if false, do not try to link to Loris
# LORIS_LIBRARIES, Loris library path
# LORIS_INCLUDE_DIRS, where to find Loris headers

include(FindPackageHandleStandardArgs)

if(APPLE)
    set(LORIS_INCLUDE_DIRS "${CMAKE_INSTALL_FULL_INCLUDEDIR}/loris")
    set(LORIS_LIBRARIES "${CMAKE_INSTALL_FULL_LIBDIR}/libloris.a")
elseif(WIN32)
    set(LORIS_INCLUDE_DIRS "C:/Program Files/loris/include")
    set(LORIS_LIBRARIES "C:/Program Files/loris/lib/loris.lib")
endif()

# Handle the QUIETLY and REQUIRED arguments
find_package_handle_standard_args(Loris DEFAULT_MSG
    LORIS_LIBRARIES
    LORIS_INCLUDE_DIRS
)

mark_as_advanced(LORIS_INCLUDE_DIRS LORIS_LIBRARIES)