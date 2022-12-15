# setting up sources
add_executable(${PROJECT_NAME} WIN32
    ./src/main.cpp
    ./src/mainwindow.cpp
    ./src/liveview.cpp
    ./src/settings.cpp
    ./src/histview.cpp
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC include
    PRIVATE src)

# Dependencies
find_package(spdlog)
find_package(cxxopts)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui OpenGLWidgets)
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
    Qt6::OpenGLWidgets
    PUBLIC
    spdlog::spdlog
    pvcam_incl
    libtiff_incl
    Common
    PvCam
    NIDAQmx_wrapper)
ELSE()
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    project_options
    project_warnings
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::OpenGLWidgets
    PUBLIC
    spdlog::spdlog
    pvcam_incl
    libtiff_incl
    Common
    PvCamD
    NIDAQmx_wrapper)
ENDIF() #WIN32

message(CMAKE_INSTALL_BINDIR=${CMAKE_INSTALL_BINDIR})
install(
    TARGETS ${PROJECT_NAME}
    COMPONENT ${PROJECT_NAME}
    RUNTIME DESTINATION       ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION       ${CMAKE_INSTALL_SHAREDSTATEDIR})


##CPack
get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

set(CPACK_PACKAGE_NAME "Nautilus")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Nautilus Installation")
set(CPACK_PACKAGE_VERSION "1.0.0") # Version of installer

message("###### CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
if(CMAKE_SYSTEM_NAME MATCHES "Windows")

    set(CPACK_PACKAGE_INSTALL_DIRECTORY "Nautilus")
    set(CPACK_NSIS_DISPLAY_NAME ${CMAKE_PACKAGE_NAME})
    set(CPACK_NSIS_COMPRESSOR lzma)
    set(CPACK_NSIS_INSTALLED_ICON_NAME Nautilus.exe)
    set(CPACK_NSIS_MENU_LINKS "Nautilus.exe" "Nautilus - Curi Bio")
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR})
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)

    include(InstallRequiredSystemLibraries)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    add_custom_command(TARGET nautilus POST_BUILD
                       COMMAND ${WINDEPLOYQT_EXECUTABLE}
                               --release
                               --verbose 1
                               --no-compiler-runtime
                               --no-translations
                               ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}.exe
    )

    install(
        DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/
        DESTINATION .
        FILES_MATCHING PATTERN "*.*"
    )
    set(CPACK_GENERATOR "ZIP;NSIS")
endif()

set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-src")
set(CPACK_SOURCE_GENERATOR "ZIP;TGZ")
include(CPack)
