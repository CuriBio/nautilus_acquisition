add_library(sqlite3_incl INTERFACE)
target_include_directories(sqlite3_incl
    INTERFACE include)

add_library(sqlite3 STATIC IMPORTED GLOBAL)
set_target_properties(sqlite3 PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/win64/sqlite3.dll")
set_target_properties(sqlite3 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")
