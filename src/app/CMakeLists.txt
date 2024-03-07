# Configure version
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/banner.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/banner.h)

set(app_icon_resource_windows "${CMAKE_SOURCE_DIR}/resources/logo.png")
# setting up sources
add_executable(${PROJECT_NAME} WIN32
    ./src/main.cpp
    ./src/mainwindow.cpp
    ./src/liveview.cpp
    ./src/settings.cpp
    ./src/histview.cpp
    ./src/stagecontrol.cpp
    ./src/advancedsetupdialog.cpp
    ./src/config.cpp
    ./src/autoupdate.cpp
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC include
    PRIVATE src)


# Dependencies
find_package(spdlog)
find_package(cxxopts)
find_package(toml11)

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG 3b15fa82ea74739b574d705fea44959b58142eb8)
FetchContent_MakeAvailable(cpr)


find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui OpenGLWidgets SvgWidgets)
qt_standard_project_setup()

# linking against the desired targets
IF (WIN32)
target_compile_options(${PROJECT_NAME} PRIVATE "/wd4996;") # disable "unknown pragma 'mark'" warnings

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    project_options
    project_warnings
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::OpenGLWidgets
    Qt6::SvgWidgets
    ordered_map
    PUBLIC
    spdlog::spdlog
    toml11::toml11
    cpr::cpr
    pvcam_incl
    libtiff_incl
    ffmpeg_incl
    libavcodec
    libavformat
    libavutil
    libswresample
    libswscale
    tango
    Common
    PvCam
    NIDAQmx_wrapper
    sqlite3_incl
    sqlite3)
ELSE()
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    project_options
    project_warnings
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::OpenGLWidgets
    Qt6::SvgWidgets
    ordered_map
    PUBLIC
    spdlog::spdlog
    toml11::toml11
    pvcam_incl
    libtiff_incl
    ffmpeg_incl
    libavcodec
    libavformat
    libavutil
    libswresample
    libswscale
    tango
    Common
    PvCamD
    NIDAQmx_wrapper)
ENDIF() #WIN32

message(CMAKE_INSTALL_BINDIR=${CMAKE_INSTALL_BINDIR})
install(
    TARGETS ${PROJECT_NAME}
    COMPONENT ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_SHAREDSTATEDIR})


##CPack
get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

set(CPACK_PACKAGE_NAME "Nautilai")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Nautilai Installation")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")

message("###### CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
if(CMAKE_SYSTEM_NAME MATCHES "Windows")

    #set(CPACK_NSIS_INSTALL_ROOT "${LocalAppData}\AppData\Local")
    set(CPACK_PACKAGE_VENDOR "CuriBio")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "Nautilai")
    set(CPACK_NSIS_DISPLAY_NAME ${CMAKE_PACKAGE_NAME})
    set(CPACK_NSIS_COMPRESSOR lzma)
    set(CPACK_NSIS_INSTALLED_ICON_NAME Nautilai.exe)
    set(CPACK_NSIS_MENU_LINKS "Nautilai.exe" "Nautilai - Curi Bio")
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR})
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)

    include(InstallRequiredSystemLibraries)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    add_custom_command(TARGET nautilai POST_BUILD
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
