add_library(libtiff_incl INTERFACE)
target_include_directories(libtiff_incl INTERFACE include)
add_library(libtiff STATIC IMPORTED GLOBAL)

IF (NOT WIN32)
set_target_properties(libtiff PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/arm64/libtiff.a")
set_target_properties(libtiff PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

ELSE()
set_target_properties(libtiff PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/win64/libtiff.lib")
set_target_properties(libtiff PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")
ENDIF()

