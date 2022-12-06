IF (NOT WIN32)
add_library(dbgdaqmx SHARED
    ./src/dbgdaqmx.cpp)

set_target_properties(dbgdaqmx PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(dbgdaqmx PROPERTIES SOVERSION 1)

target_include_directories(dbgdaqmx
    PUBLIC include
    PRIVATE src)

#find_package(Boost COMPONENTS log REQUIRED)
find_package(spdlog)

# linking against the desired targets
target_link_libraries(dbgdaqmx
    PRIVATE
    project_options
    project_warnings
    nidaqmx_incl
    spdlog::spdlog)

install(TARGETS dbgdaqmx
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/
    RUNTIME DESTINATION       ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION       ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION      ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/
    ARCHIVE DESTINATION       ${CMAKE_INSTALL_SHAREDSTATEDIR})
ENDIF() #WIN32
