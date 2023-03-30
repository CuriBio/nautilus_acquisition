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
    target_link_libraries(${TILE_PROJECT}
    PRIVATE
    project_options
    project_warnings
    PUBLIC
    spdlog::spdlog
    toml11::toml11
    libtiff_incl
    Common)
ELSE()
    target_link_libraries(${TILE_PROJECT}
    PRIVATE
    project_options
    project_warnings
    PUBLIC
    spdlog::spdlog
    toml11::toml11
    libtiff_incl
    Common)
ENDIF() #WIN32

message(CMAKE_INSTALL_BINDIR=${CMAKE_INSTALL_BINDIR})
install(
    TARGETS ${TILE_PROJECT}
    COMPONENT ${TILE_PROJECT}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_SHAREDSTATEDIR})
