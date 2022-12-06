#find logging library
find_package(spdlog)

add_library(NIDAQmx_wrapper SHARED
    ./src/NIDAQmx.cpp)

set_target_properties(NIDAQmx_wrapper PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(NIDAQmx_wrapper PROPERTIES SOVERSION 1)

target_include_directories(NIDAQmx_wrapper
    PUBLIC include
    PRIVATE src)

IF (WIN32)
message("Building NIDAQmx_wrapper for WIN32")
target_link_libraries(NIDAQmx_wrapper
    PRIVATE
    project_options
    project_warnings
    spdlog::spdlog
    nidaqmx_incl
    nidaqmx)
ELSE()
message("Building NIDAQmx_wrapper for debug")
target_link_libraries(NIDAQmx_wrapper
    PRIVATE
    project_options
    project_warnings
    spdlog::spdlog
    nidaqmx_incl
    dbgdaqmx)
ENDIF()

install(TARGETS NIDAQmx_wrapper
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/
    RUNTIME DESTINATION       ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION       ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION      ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/
    ARCHIVE DESTINATION       ${CMAKE_INSTALL_SHAREDSTATEDIR})
