

# Conan automatically generated toolchain file
# DO NOT EDIT MANUALLY, it will be overwritten

# Avoid including toolchain file several times (bad if appending to variables like
#   CMAKE_CXX_FLAGS. See https://github.com/android/ndk/issues/323
include_guard()

message(STATUS "Using Conan toolchain: ${CMAKE_CURRENT_LIST_FILE}")

if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeToolchain' generator only works with CMake >= 3.15")
endif()






set(CMAKE_GENERATOR_PLATFORM "x64" CACHE STRING "" FORCE)






# Definition of VS runtime, defined from build_type, compiler.runtime, compiler.runtime_type
cmake_policy(GET CMP0091 POLICY_CMP0091)
if(NOT "${POLICY_CMP0091}" STREQUAL NEW)
    message(FATAL_ERROR "The CMake policy CMP0091 must be NEW, but is '${POLICY_CMP0091}'")
endif()
set(CMAKE_MSVC_RUNTIME_LIBRARY "$<$<CONFIG:Release>:MultiThreadedDLL>")

string(APPEND CONAN_CXX_FLAGS " /MP2")
string(APPEND CONAN_C_FLAGS " /MP2")

# Extra c, cxx, linkflags and defines


if(DEFINED CONAN_CXX_FLAGS)
  string(APPEND CMAKE_CXX_FLAGS_INIT " ${CONAN_CXX_FLAGS}")
endif()
if(DEFINED CONAN_C_FLAGS)
  string(APPEND CMAKE_C_FLAGS_INIT " ${CONAN_C_FLAGS}")
endif()
if(DEFINED CONAN_SHARED_LINKER_FLAGS)
  string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " ${CONAN_SHARED_LINKER_FLAGS}")
endif()
if(DEFINED CONAN_EXE_LINKER_FLAGS)
  string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " ${CONAN_EXE_LINKER_FLAGS}")
endif()

get_property( _CMAKE_IN_TRY_COMPILE GLOBAL PROPERTY IN_TRY_COMPILE )
if(_CMAKE_IN_TRY_COMPILE)
    message(STATUS "Running toolchain IN_TRY_COMPILE")
    return()
endif()

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)

# Definition of CMAKE_MODULE_PATH
# The root (which is the default builddirs) path of dependencies in the host context
list(PREPEND CMAKE_MODULE_PATH "D:/Users/nautilus/.conan/data/cxxopts/3.0.0/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/" "D:/Users/nautilus/.conan/data/toml11/3.7.1/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/")
# the generators folder (where conan generates files, like this toolchain)
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# Definition of CMAKE_PREFIX_PATH, CMAKE_XXXXX_PATH
# The Conan local "generators" folder, where this toolchain is saved.
list(PREPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR} )
list(PREPEND CMAKE_PROGRAM_PATH "D:/Users/nautilus/.conan/data/spdlog/1.10.0/_/_/package/70a17ef929dad20e1afdd6cf274326a965e13481/bin" "D:/Users/nautilus/.conan/data/fmt/8.1.1/_/_/package/8c0a53bc9b0a08b9311ca89254091ba7fb4f89b4/bin")
list(PREPEND CMAKE_LIBRARY_PATH "D:/Users/nautilus/.conan/data/spdlog/1.10.0/_/_/package/70a17ef929dad20e1afdd6cf274326a965e13481/lib" "D:/Users/nautilus/.conan/data/fmt/8.1.1/_/_/package/8c0a53bc9b0a08b9311ca89254091ba7fb4f89b4/lib")
list(PREPEND CMAKE_INCLUDE_PATH "D:/Users/nautilus/.conan/data/spdlog/1.10.0/_/_/package/70a17ef929dad20e1afdd6cf274326a965e13481/include" "D:/Users/nautilus/.conan/data/cxxopts/3.0.0/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/include" "D:/Users/nautilus/.conan/data/toml11/3.7.1/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/include" "D:/Users/nautilus/.conan/data/toml11/3.7.1/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/include/toml11" "D:/Users/nautilus/.conan/data/fmt/8.1.1/_/_/package/8c0a53bc9b0a08b9311ca89254091ba7fb4f89b4/include")



if (DEFINED ENV{PKG_CONFIG_PATH})
set(ENV{PKG_CONFIG_PATH} "D:/Users/nautilus/Desktop/nautilus_acquisition/build-R;$ENV{PKG_CONFIG_PATH}")
else()
set(ENV{PKG_CONFIG_PATH} "D:/Users/nautilus/Desktop/nautilus_acquisition/build-R;")
endif()




# Variables
# Variables  per configuration


# Preprocessor definitions
# Preprocessor definitions per configuration
