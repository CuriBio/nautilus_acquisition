cmake_minimum_required(VERSION 3.10)
set(NAUTILAI_VERSION 0.2.4)

project(nautilai LANGUAGES CXX C VERSION ${NAUTILAI_VERSION})
set(PROJECT_BRIEF "Nautilai Acquisition")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib/")
set(CMAKE_INSTALL_BINDIR .)

message(CMAKE_LIBRARY_OUTPUT_DIRECTORY=${CMAKE_LIBRARY_OUTPUT_DIRECTORY})


# get conan.cmake
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD  "https://raw.githubusercontent.com/conan-io/cmake-conan/b240c80/conan.cmake"
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
set(CMAKE_CXX_EXCEPTIONS OFF)
set(CMAKE_CXX_RTII OFF)

add_library(project_warnings INTERFACE)
add_library(project_options INTERFACE)

target_compile_features(project_options INTERFACE cxx_std_23)
if (WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(
        /O2
        /Oi
        /arch:AVX2
    )
ENDIF()

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

#Install NSIS template
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/NSIS.template.in DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#Install config file
install(FILES ${CMAKE_SOURCE_DIR}/resources/nautilai.toml DESTINATION ${CMAKE_INSTALL_BINDIR})

#install stylesheet
install(FILES ${CMAKE_SOURCE_DIR}/resources/curibio.qss DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)

#install icons
install(FILES ${CMAKE_SOURCE_DIR}/resources/up1.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/up1_active.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/up2.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/up2_active.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/up3.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/up3_active.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)

install(FILES ${CMAKE_SOURCE_DIR}/resources/down1.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/down1_active.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/down2.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/down2_active.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/down3.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/down3_active.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)

install(FILES ${CMAKE_SOURCE_DIR}/resources/right1.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/right1_active.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/right2.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/right2_active.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/right3.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/right3_active.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)

install(FILES ${CMAKE_SOURCE_DIR}/resources/left1.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/left1_active.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/left2.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/left2_active.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/left3.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/left3_active.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)

install(FILES ${CMAKE_SOURCE_DIR}/resources/logo.png DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)

#loading gif
install(FILES ${CMAKE_SOURCE_DIR}/resources/loading.gif DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)

#install local_analysis.exe
install(FILES ${CMAKE_SOURCE_DIR}/resources/local_analysis.exe DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)

#install plate formats folder
install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/plate_formats DESTINATION ${CMAKE_INSTALL_BINDIR}/)

install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_plate-base.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_24-well-plate-inactive.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_24-well-plate-section1-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_24-well-plate-section2-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_24-well-plate-section3-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_24-well-plate-section4-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_24-well-plate-section5-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_24-well-plate-section6-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_96-well-plate-round-inactive.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_96-well-plate-round-section1-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_96-well-plate-round-section2-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_96-well-plate-round-section3-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_96-well-plate-round-section4-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_96-well-plate-round-section5-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_96-well-plate-round-section6-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_96-well-plate-square-inactive.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_96-well-plate-square-section1-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_96-well-plate-square-section2-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_96-well-plate-square-section3-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_96-well-plate-square-section4-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_96-well-plate-square-section5-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_96-well-plate-square-section6-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_384-well-plate-circle-inactive.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_384-well-plate-circle-section1-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_384-well-plate-circle-section2-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_384-well-plate-circle-section3-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_384-well-plate-circle-section4-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_384-well-plate-circle-section5-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_384-well-plate-circle-section6-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_384-well-plate-square-inactive.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_384-well-plate-square-section6-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources/)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_384-well-plate-square-section1-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_384-well-plate-square-section2-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_384-well-plate-square-section3-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_384-well-plate-square-section4-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Nautilus-software_384-well-plate-square-section5-active.svg DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)

add_subdirectory(libs)
add_subdirectory(src)
