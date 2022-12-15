#find logging library
find_package(spdlog)

IF (NOT WIN32)
add_library(PvCamD STATIC
    ./src/pm/Camera.cpp
    ./src/pm/Frame.cpp
    ./src/pm/Acquisition.cpp)

set_target_properties(PvCamD PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(PvCamD PROPERTIES SOVERSION 1)

target_include_directories(PvCamD
    PUBLIC include
    PRIVATE src)

target_link_libraries(PvCamD
    PRIVATE
    project_options
    project_warnings
    pvcam_incl
    dbgcam
    libtiff
    libtiff_incl
    Common
    spdlog::spdlog)
ENDIF() #NOT WIN32

IF (WIN32)
add_library(PvCam STATIC
    ./src/pm/Camera.cpp
    ./src/pm/Frame.cpp
    ./src/pm/Acquisition.cpp)

set_target_properties(PvCam PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(PvCam PROPERTIES SOVERSION 1)

target_include_directories(PvCam
    PUBLIC include
    PRIVATE src)

target_link_libraries(PvCam
    PRIVATE
    project_options
    project_warnings
    pvcam_incl
    libtiff
    libtiff_incl
    pvcam64
    Common
    spdlog::spdlog)
ENDIF() #WIN32
