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
#Boost::log)


install(TARGETS dbgcam
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/
    RUNTIME DESTINATION       ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION       ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION      ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/
    ARCHIVE DESTINATION       ${CMAKE_INSTALL_SHAREDSTATEDIR})
ENDIF()
