# FindSdif.cmake
# Locate SDIF library
# This module defines
# Sdif::Sdif
# SDIF_FOUND, if false, do not try to link to SDIF
# SDIF_INCLUDE_DIR, where to find SDIF headers

include(FindPackageHandleStandardArgs)

if(APPLE)
    find_path(SDIF_INCLUDE_DIR SDIF.h
        PATHS
        /Library/Frameworks/SDIF.framework/Headers
        /Library/Frameworks/SDIF.framework/Versions/Current/Headers
    )

    find_library(SDIF_LIBRARY
        NAMES SDIF
        PATHS
        /Library/Frameworks
    )
else()
    # Add Windows/Linux paths here if needed
endif()

find_package_handle_standard_args(Sdif
    REQUIRED_VARS SDIF_LIBRARY SDIF_INCLUDE_DIR
)

if(SDIF_FOUND AND NOT TARGET Sdif::Sdif)
    add_library(Sdif::Sdif INTERFACE IMPORTED)
    set_target_properties(Sdif::Sdif PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${SDIF_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "${SDIF_LIBRARY}"
    )
endif()

mark_as_advanced(SDIF_INCLUDE_DIR SDIF_LIBRARY)