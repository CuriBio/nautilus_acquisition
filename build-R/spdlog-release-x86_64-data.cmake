########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

list(APPEND spdlog_COMPONENT_NAMES spdlog::spdlog)
list(REMOVE_DUPLICATES spdlog_COMPONENT_NAMES)
list(APPEND spdlog_FIND_DEPENDENCY_NAMES fmt)
list(REMOVE_DUPLICATES spdlog_FIND_DEPENDENCY_NAMES)
set(fmt_FIND_MODE "NO_MODULE")

########### VARIABLES #######################################################################
#############################################################################################
set(spdlog_PACKAGE_FOLDER_RELEASE "D:/Users/nautilus/.conan/data/spdlog/1.10.0/_/_/package/70a17ef929dad20e1afdd6cf274326a965e13481")
set(spdlog_BUILD_MODULES_PATHS_RELEASE )


set(spdlog_INCLUDE_DIRS_RELEASE "${spdlog_PACKAGE_FOLDER_RELEASE}/include")
set(spdlog_RES_DIRS_RELEASE )
set(spdlog_DEFINITIONS_RELEASE "-DSPDLOG_FMT_EXTERNAL"
			"-DSPDLOG_COMPILED_LIB")
set(spdlog_SHARED_LINK_FLAGS_RELEASE )
set(spdlog_EXE_LINK_FLAGS_RELEASE )
set(spdlog_OBJECTS_RELEASE )
set(spdlog_COMPILE_DEFINITIONS_RELEASE "SPDLOG_FMT_EXTERNAL"
			"SPDLOG_COMPILED_LIB")
set(spdlog_COMPILE_OPTIONS_C_RELEASE )
set(spdlog_COMPILE_OPTIONS_CXX_RELEASE )
set(spdlog_LIB_DIRS_RELEASE "${spdlog_PACKAGE_FOLDER_RELEASE}/lib")
set(spdlog_LIBS_RELEASE spdlog)
set(spdlog_SYSTEM_LIBS_RELEASE )
set(spdlog_FRAMEWORK_DIRS_RELEASE )
set(spdlog_FRAMEWORKS_RELEASE )
set(spdlog_BUILD_DIRS_RELEASE )

# COMPOUND VARIABLES
set(spdlog_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${spdlog_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${spdlog_COMPILE_OPTIONS_C_RELEASE}>")
set(spdlog_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${spdlog_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${spdlog_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${spdlog_EXE_LINK_FLAGS_RELEASE}>")


set(spdlog_COMPONENTS_RELEASE spdlog::spdlog)
########### COMPONENT spdlog::spdlog VARIABLES ############################################

set(spdlog_spdlog_spdlog_INCLUDE_DIRS_RELEASE "${spdlog_PACKAGE_FOLDER_RELEASE}/include")
set(spdlog_spdlog_spdlog_LIB_DIRS_RELEASE "${spdlog_PACKAGE_FOLDER_RELEASE}/lib")
set(spdlog_spdlog_spdlog_RES_DIRS_RELEASE )
set(spdlog_spdlog_spdlog_DEFINITIONS_RELEASE "-DSPDLOG_FMT_EXTERNAL"
			"-DSPDLOG_COMPILED_LIB")
set(spdlog_spdlog_spdlog_OBJECTS_RELEASE )
set(spdlog_spdlog_spdlog_COMPILE_DEFINITIONS_RELEASE "SPDLOG_FMT_EXTERNAL"
			"SPDLOG_COMPILED_LIB")
set(spdlog_spdlog_spdlog_COMPILE_OPTIONS_C_RELEASE "")
set(spdlog_spdlog_spdlog_COMPILE_OPTIONS_CXX_RELEASE "")
set(spdlog_spdlog_spdlog_LIBS_RELEASE spdlog)
set(spdlog_spdlog_spdlog_SYSTEM_LIBS_RELEASE )
set(spdlog_spdlog_spdlog_FRAMEWORK_DIRS_RELEASE )
set(spdlog_spdlog_spdlog_FRAMEWORKS_RELEASE )
set(spdlog_spdlog_spdlog_DEPENDENCIES_RELEASE fmt::fmt)
set(spdlog_spdlog_spdlog_SHARED_LINK_FLAGS_RELEASE )
set(spdlog_spdlog_spdlog_EXE_LINK_FLAGS_RELEASE )
# COMPOUND VARIABLES
set(spdlog_spdlog_spdlog_LINKER_FLAGS_RELEASE
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${spdlog_spdlog_spdlog_SHARED_LINK_FLAGS_RELEASE}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${spdlog_spdlog_spdlog_SHARED_LINK_FLAGS_RELEASE}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${spdlog_spdlog_spdlog_EXE_LINK_FLAGS_RELEASE}>
)
set(spdlog_spdlog_spdlog_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${spdlog_spdlog_spdlog_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${spdlog_spdlog_spdlog_COMPILE_OPTIONS_C_RELEASE}>")