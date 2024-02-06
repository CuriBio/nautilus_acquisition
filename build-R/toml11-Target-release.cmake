# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(toml11_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(toml11_FRAMEWORKS_FOUND_RELEASE "${toml11_FRAMEWORKS_RELEASE}" "${toml11_FRAMEWORK_DIRS_RELEASE}")

set(toml11_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET toml11_DEPS_TARGET)
    add_library(toml11_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET toml11_DEPS_TARGET
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${toml11_FRAMEWORKS_FOUND_RELEASE}>
             $<$<CONFIG:Release>:${toml11_SYSTEM_LIBS_RELEASE}>
             $<$<CONFIG:Release>:>
             APPEND)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### toml11_DEPS_TARGET to all of them
conan_package_library_targets("${toml11_LIBS_RELEASE}"    # libraries
                              "${toml11_LIB_DIRS_RELEASE}" # package_libdir
                              toml11_DEPS_TARGET
                              toml11_LIBRARIES_TARGETS  # out_libraries_targets
                              "_RELEASE"
                              "toml11")    # package_name

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${toml11_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})

########## GLOBAL TARGET PROPERTIES Release ########################################
    set_property(TARGET toml11::toml11
                 PROPERTY INTERFACE_LINK_LIBRARIES
                 $<$<CONFIG:Release>:${toml11_OBJECTS_RELEASE}>
                 $<$<CONFIG:Release>:${toml11_LIBRARIES_TARGETS}>
                 APPEND)

    if("${toml11_LIBS_RELEASE}" STREQUAL "")
        # If the package is not declaring any "cpp_info.libs" the package deps, system libs,
        # frameworks etc are not linked to the imported targets and we need to do it to the
        # global target
        set_property(TARGET toml11::toml11
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     toml11_DEPS_TARGET
                     APPEND)
    endif()

    set_property(TARGET toml11::toml11
                 PROPERTY INTERFACE_LINK_OPTIONS
                 $<$<CONFIG:Release>:${toml11_LINKER_FLAGS_RELEASE}> APPEND)
    set_property(TARGET toml11::toml11
                 PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                 $<$<CONFIG:Release>:${toml11_INCLUDE_DIRS_RELEASE}> APPEND)
    set_property(TARGET toml11::toml11
                 PROPERTY INTERFACE_COMPILE_DEFINITIONS
                 $<$<CONFIG:Release>:${toml11_COMPILE_DEFINITIONS_RELEASE}> APPEND)
    set_property(TARGET toml11::toml11
                 PROPERTY INTERFACE_COMPILE_OPTIONS
                 $<$<CONFIG:Release>:${toml11_COMPILE_OPTIONS_RELEASE}> APPEND)

########## For the modules (FindXXX)
set(toml11_LIBRARIES_RELEASE toml11::toml11)
