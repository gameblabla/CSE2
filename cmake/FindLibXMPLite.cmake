# Exports...
# * libxmp-lite_FOUND
# * LIBXMPLITE_INCLUDE_DIRS
# * LIBXMPLITE_LIBRARIES
# * libxmp-lite

find_path(LIBXMPLITE_INCLUDE_DIRS xmp.h PATH_SUFFIXES libxmp-lite)
find_library(LIBXMPLITE_LIBRARIES xmp-lite)
mark_as_advanced(LIBXMPLITE_INCLUDE_DIRS LIBXMPLITE_LIBRARIES)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libxmp-lite DEFAULT_MSG
	LIBXMPLITE_INCLUDE_DIRS LIBXMPLITE_LIBRARIES)

if(libxmp-lite_FOUND AND NOT TARGET libxmp-lite)
  add_library(libxmp-lite UNKNOWN IMPORTED)
  set_target_properties(libxmp-lite PROPERTIES
    IMPORTED_LOCATION "${LIBXMPLITE_LIBRARIES}"
    INTERFACE_INCLUDE_DIRECTORIES "${LIBXMPLITE_INCLUDE_DIRS}"
  )
endif()
