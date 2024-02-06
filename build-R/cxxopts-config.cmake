########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

if(cxxopts_FIND_QUIETLY)
    set(cxxopts_MESSAGE_MODE VERBOSE)
else()
    set(cxxopts_MESSAGE_MODE STATUS)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cxxoptsTargets.cmake)
include(CMakeFindDependencyMacro)

check_build_type_defined()

foreach(_DEPENDENCY ${cxxopts_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(cxxopts_VERSION_STRING "3.0.0")
set(cxxopts_INCLUDE_DIRS ${cxxopts_INCLUDE_DIRS_RELEASE} )
set(cxxopts_INCLUDE_DIR ${cxxopts_INCLUDE_DIRS_RELEASE} )
set(cxxopts_LIBRARIES ${cxxopts_LIBRARIES_RELEASE} )
set(cxxopts_DEFINITIONS ${cxxopts_DEFINITIONS_RELEASE} )

# Only the first installed configuration is included to avoid the collision
foreach(_BUILD_MODULE ${cxxopts_BUILD_MODULES_PATHS_RELEASE} )
    message(${cxxopts_MESSAGE_MODE} "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


