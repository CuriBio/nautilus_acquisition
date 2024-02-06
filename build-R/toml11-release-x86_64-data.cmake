########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(toml11_COMPONENT_NAMES "")
set(toml11_FIND_DEPENDENCY_NAMES "")

########### VARIABLES #######################################################################
#############################################################################################
set(toml11_PACKAGE_FOLDER_RELEASE "D:/Users/nautilus/.conan/data/toml11/3.7.1/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9")
set(toml11_BUILD_MODULES_PATHS_RELEASE )


set(toml11_INCLUDE_DIRS_RELEASE "${toml11_PACKAGE_FOLDER_RELEASE}/include"
			"${toml11_PACKAGE_FOLDER_RELEASE}/include/toml11")
set(toml11_RES_DIRS_RELEASE )
set(toml11_DEFINITIONS_RELEASE )
set(toml11_SHARED_LINK_FLAGS_RELEASE )
set(toml11_EXE_LINK_FLAGS_RELEASE )
set(toml11_OBJECTS_RELEASE )
set(toml11_COMPILE_DEFINITIONS_RELEASE )
set(toml11_COMPILE_OPTIONS_C_RELEASE )
set(toml11_COMPILE_OPTIONS_CXX_RELEASE )
set(toml11_LIB_DIRS_RELEASE )
set(toml11_LIBS_RELEASE )
set(toml11_SYSTEM_LIBS_RELEASE )
set(toml11_FRAMEWORK_DIRS_RELEASE )
set(toml11_FRAMEWORKS_RELEASE )
set(toml11_BUILD_DIRS_RELEASE "${toml11_PACKAGE_FOLDER_RELEASE}/")

# COMPOUND VARIABLES
set(toml11_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${toml11_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${toml11_COMPILE_OPTIONS_C_RELEASE}>")
set(toml11_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${toml11_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${toml11_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${toml11_EXE_LINK_FLAGS_RELEASE}>")


set(toml11_COMPONENTS_RELEASE )