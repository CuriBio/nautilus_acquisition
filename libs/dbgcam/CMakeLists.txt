IF (NOT WIN32)
add_library(dbgcam SHARED
    ./src/dbgcam.cpp)

set_target_properties(dbgcam PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(dbgcam PROPERTIES SOVERSION 1)

target_include_directories(dbgcam
    PUBLIC include
    PRIVATE src)

#find_package(Boost COMPONENTS log REQUIRED)
find_package(spdlog)

# linking against the desired targets
target_link_libraries(dbgcam
    PRIVATE
    project_options
    project_warnings
    pvcam_incl
    spdlog::spdlog)
ENDIF() #WIN32
