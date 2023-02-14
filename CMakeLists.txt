cmake_minimum_required(VERSION 3.10)
set(NAUTILUS_VERSION 0.1.7)

project(nautilus LANGUAGES CXX C VERSION ${NAUTILUS_VERSION})
set(PROJECT_BRIEF "Nautilus Acquisition")


set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib/")
set(CMAKE_INSTALL_BINDIR .)

message(CMAKE_LIBRARY_OUTPUT_DIRECTORY=${CMAKE_LIBRARY_OUTPUT_DIRECTORY})


# get conan.cmake
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD  "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
        "${CMAKE_BINARY_DIR}/conan.cmake")
endif()
include(${CMAKE_BINARY_DIR}/conan.cmake)

# setup conan
conan_check(REQUIRED)
conan_cmake_run(
    CONANFILE
    conanfile.txt
    BASIC_SETUP
    CONAN_COMMAND
    ${CONAN_CMD}
    CMAKE_TARGETS
    BUILD
    missing)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Qt6 install
IF (WIN32)
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libs/qt6/6.4.1/msvc2019_64/")
ELSE()
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libs/qt6/6.4.1/")
ENDIF()
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

message("########## ${PROJECT_NAME}:${PROJECT_VERSION} ##########")

set(CMAKE_CXX_STANDARD 23)
add_library(project_warnings INTERFACE)
add_library(project_options INTERFACE)

target_compile_features(project_options INTERFACE cxx_std_23)
# set(CMAKE_CXX_FLAGS "-Wall")
# set(CMAKE_CXX_FLAGS_DEBUG "-g")
# set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
#link_libraries("-lm")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(PROJECT_BUILD ${CMAKE_CURRENT_BINARY_DIR})

#Install license.txt
install(FILES LICENSE.txt DESTINATION ${CMAKE_INSTALL_BINDIR})

#Install config file
install(FILES ${CMAKE_SOURCE_DIR}/resources/nautilus.toml DESTINATION ${CMAKE_INSTALL_BINDIR})

#install icons
install(FILES ${CMAKE_SOURCE_DIR}/resources/up1.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/up2.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/up3.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)

install(FILES ${CMAKE_SOURCE_DIR}/resources/down1.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/down2.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/down3.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)

install(FILES ${CMAKE_SOURCE_DIR}/resources/right1.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/right2.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/right3.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)

install(FILES ${CMAKE_SOURCE_DIR}/resources/left1.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/left2.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/left3.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)

add_subdirectory(libs)
add_subdirectory(src)
