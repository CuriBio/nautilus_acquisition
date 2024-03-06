#find logging library
find_package(spdlog)

add_library(Common STATIC
./src/Bitmap.cpp
./src/BitmapFormat.cpp
./src/TiffFile.cpp)

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
    libtiff
    sqlite3_incl
    sqlite3
    )
