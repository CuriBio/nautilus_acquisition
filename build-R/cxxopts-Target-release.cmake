# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(cxxopts_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(cxxopts_FRAMEWORKS_FOUND_RELEASE "${cxxopts_FRAMEWORKS_RELEASE}" "${cxxopts_FRAMEWORK_DIRS_RELEASE}")

set(cxxopts_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET cxxopts_DEPS_TARGET)
    add_library(cxxopts_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET cxxopts_DEPS_TARGET
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${cxxopts_FRAMEWORKS_FOUND_RELEASE}>
             $<$<CONFIG:Release>:${cxxopts_SYSTEM_LIBS_RELEASE}>
             $<$<CONFIG:Release>:>
             APPEND)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### cxxopts_DEPS_TARGET to all of them
conan_package_library_targets("${cxxopts_LIBS_RELEASE}"    # libraries
                              "${cxxopts_LIB_DIRS_RELEASE}" # package_libdir
                              cxxopts_DEPS_TARGET
                              cxxopts_LIBRARIES_TARGETS  # out_libraries_targets
                              "_RELEASE"
                              "cxxopts")    # package_name

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${cxxopts_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})

########## GLOBAL TARGET PROPERTIES Release ########################################
    set_property(TARGET cxxopts::cxxopts
                 PROPERTY INTERFACE_LINK_LIBRARIES
                 $<$<CONFIG:Release>:${cxxopts_OBJECTS_RELEASE}>
                 $<$<CONFIG:Release>:${cxxopts_LIBRARIES_TARGETS}>
                 APPEND)

    if("${cxxopts_LIBS_RELEASE}" STREQUAL "")
        # If the package is not declaring any "cpp_info.libs" the package deps, system libs,
        # frameworks etc are not linked to the imported targets and we need to do it to the
        # global target
        set_property(TARGET cxxopts::cxxopts
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     cxxopts_DEPS_TARGET
                     APPEND)
    endif()

    set_property(TARGET cxxopts::cxxopts
                 PROPERTY INTERFACE_LINK_OPTIONS
                 $<$<CONFIG:Release>:${cxxopts_LINKER_FLAGS_RELEASE}> APPEND)
    set_property(TARGET cxxopts::cxxopts
                 PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                 $<$<CONFIG:Release>:${cxxopts_INCLUDE_DIRS_RELEASE}> APPEND)
    set_property(TARGET cxxopts::cxxopts
                 PROPERTY INTERFACE_COMPILE_DEFINITIONS
                 $<$<CONFIG:Release>:${cxxopts_COMPILE_DEFINITIONS_RELEASE}> APPEND)
    set_property(TARGET cxxopts::cxxopts
                 PROPERTY INTERFACE_COMPILE_OPTIONS
                 $<$<CONFIG:Release>:${cxxopts_COMPILE_OPTIONS_RELEASE}> APPEND)

########## For the modules (FindXXX)
set(cxxopts_LIBRARIES_RELEASE cxxopts::cxxopts)
