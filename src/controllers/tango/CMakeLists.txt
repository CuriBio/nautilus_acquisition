#find logging library
find_package(spdlog)

add_library(tango STATIC
    ./src/TangoStage.cpp)

set_target_properties(tango PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(tango PROPERTIES SOVERSION 1)

target_include_directories(tango
    PUBLIC include
    PRIVATE src)

IF (WIN32)
message("Building NIDAQmx_wrapper for WIN32")
target_link_libraries(tango
    PRIVATE
    project_options
    project_warnings
    spdlog::spdlog
    tango_incl
    tango_lib)
ELSE()
message("Building NIDAQmx_wrapper for debug")
target_link_libraries(tango
    PRIVATE
    project_options
    project_warnings
    spdlog::spdlog
    tango_incl
    tango_lib)
ENDIF()
