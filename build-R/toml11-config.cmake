########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

if(toml11_FIND_QUIETLY)
    set(toml11_MESSAGE_MODE VERBOSE)
else()
    set(toml11_MESSAGE_MODE STATUS)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/toml11Targets.cmake)
include(CMakeFindDependencyMacro)

check_build_type_defined()

foreach(_DEPENDENCY ${toml11_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(toml11_VERSION_STRING "3.7.1")
set(toml11_INCLUDE_DIRS ${toml11_INCLUDE_DIRS_RELEASE} )
set(toml11_INCLUDE_DIR ${toml11_INCLUDE_DIRS_RELEASE} )
set(toml11_LIBRARIES ${toml11_LIBRARIES_RELEASE} )
set(toml11_DEFINITIONS ${toml11_DEFINITIONS_RELEASE} )

# Only the first installed configuration is included to avoid the collision
foreach(_BUILD_MODULE ${toml11_BUILD_MODULES_PATHS_RELEASE} )
    message(${toml11_MESSAGE_MODE} "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


