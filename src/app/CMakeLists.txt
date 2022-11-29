# setting up sources
add_executable(${PROJECT_NAME}
    ./src/main.cpp
    ./src/mainwindow.cpp
    ./src/liveview.cpp
    ./src/settings.cpp
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC include
    PRIVATE src)

# Dependencies
find_package(spdlog)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)
qt_standard_project_setup()


# linking against the desired targets
IF (WIN32)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    project_options
    project_warnings
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    PUBLIC
    spdlog::spdlog
    pvcam_incl
    libtiff_incl
    Common
    PvCam)
ELSE()
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    project_options
    project_warnings
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    PUBLIC
    spdlog::spdlog
    pvcam_incl
    libtiff_incl
    Common
    PvCamD
    )
ENDIF() #WIN32

message(CMAKE_INSTALL_BINDIR=${CMAKE_INSTALL_BINDIR})
install(TARGETS ${PROJECT_NAME}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/
    RUNTIME DESTINATION       ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION       ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION      ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/
    ARCHIVE DESTINATION       ${CMAKE_INSTALL_SHAREDSTATEDIR})
