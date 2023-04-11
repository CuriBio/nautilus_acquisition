set(TILE_PROJECT "tile")
# setting up sources
add_executable(${TILE_PROJECT}
    ./src/main.cpp
    )

target_include_directories(${TILE_PROJECT}
    PUBLIC include
    PRIVATE src)


# Dependencies
find_package(spdlog)
find_package(cxxopts)
find_package(toml11)

# linking against the desired targets
IF (WIN32)
    set_property(TARGET ${TILE_PROJECT} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
    target_link_libraries(${TILE_PROJECT}
    PRIVATE
    project_options
    project_warnings
    ordered_map
    PUBLIC
    bcrypt.lib
    spdlog::spdlog
    toml11::toml11
    libtiff_incl
    ffmpeg_incl
    libavcodec
    libavformat
    libavutil
    libswresample
    libswscale
    Common)
ELSE()
    target_link_libraries(${TILE_PROJECT}
    PRIVATE
    project_options
    project_warnings
    ordered_map
    PUBLIC
    spdlog::spdlog
    toml11::toml11
    libz_incl
    libz
    libtiff_incl
    ffmpeg_incl
    libavcodec
    libavformat
    libavutil
    libswresample
    libswscale
    Common)
ENDIF() #WIN32

message(CMAKE_INSTALL_BINDIR=${CMAKE_INSTALL_BINDIR})
install(
    TARGETS ${TILE_PROJECT}
    COMPONENT ${TILE_PROJECT}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_SHAREDSTATEDIR})

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR})
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
    include(InstallRequiredSystemLibraries)
endif()
