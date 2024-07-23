#find logging library
find_package(spdlog)
find_package(sqlite3)

add_library(Common STATIC
./src/Bitmap.cpp
./src/BitmapFormat.cpp
./src/TiffFile.cpp
./src/Rois.cpp)

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
    SQLite::SQLite3
    )
