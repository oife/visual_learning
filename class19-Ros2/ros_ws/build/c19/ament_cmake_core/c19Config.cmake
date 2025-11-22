# generated from ament/cmake/core/templates/nameConfig.cmake.in

# prevent multiple inclusion
if(_c19_CONFIG_INCLUDED)
  # ensure to keep the found flag the same
  if(NOT DEFINED c19_FOUND)
    # explicitly set it to FALSE, otherwise CMake will set it to TRUE
    set(c19_FOUND FALSE)
  elseif(NOT c19_FOUND)
    # use separate condition to avoid uninitialized variable warning
    set(c19_FOUND FALSE)
  endif()
  return()
endif()
set(_c19_CONFIG_INCLUDED TRUE)

# output package information
if(NOT c19_FIND_QUIETLY)
  message(STATUS "Found c19: 0.0.0 (${c19_DIR})")
endif()

# warn when using a deprecated package
if(NOT "" STREQUAL "")
  set(_msg "Package 'c19' is deprecated")
  # append custom deprecation text if available
  if(NOT "" STREQUAL "TRUE")
    set(_msg "${_msg} ()")
  endif()
  # optionally quiet the deprecation message
  if(NOT ${c19_DEPRECATED_QUIET})
    message(DEPRECATION "${_msg}")
  endif()
endif()

# flag package as ament-based to distinguish it after being find_package()-ed
set(c19_FOUND_AMENT_PACKAGE TRUE)

# include all config extra files
set(_extras "")
foreach(_extra ${_extras})
  include("${c19_DIR}/${_extra}")
endforeach()
