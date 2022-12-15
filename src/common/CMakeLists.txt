#find logging library
find_package(spdlog)

add_library(Common STATIC
./src/PMemCopy.cpp
./src/Bitmap.cpp
./src/BitmapFormat.cpp
./src/FileWriter.cpp
./src/PMemCopy.cpp)

set_target_properties(Common PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(Common PROPERTIES SOVERSION 1)
add_definitions(-DTIFF_DISABLE_DEPRECATED)

target_include_directories(Common
    PUBLIC include
    PRIVATE src)

target_link_libraries(Common
    PRIVATE
    project_options
    project_warnings
    spdlog::spdlog
    libtiff_incl
    libtiff)

install(TARGETS Common
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/
    RUNTIME DESTINATION       ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION       ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION      ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/
    ARCHIVE DESTINATION       ${CMAKE_INSTALL_SHAREDSTATEDIR})
