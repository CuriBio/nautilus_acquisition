add_library(libz_incl INTERFACE)
target_include_directories(libz_incl INTERFACE include)
add_library(libz STATIC IMPORTED GLOBAL)

IF (NOT WIN32)
set_target_properties(libz PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/arm64/libz.a")
set_target_properties(libz PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")
ELSE()
ENDIF()


