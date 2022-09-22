add_library(libtiff_incl INTERFACE)
target_include_directories(libtiff_incl INTERFACE include)
add_library(libtiff STATIC IMPORTED GLOBAL)

IF (NOT WIN32)
set_target_properties(libtiff PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/arm64/libtiff.a")
set_target_properties(libtiff PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

file(COPY bin/arm64/libtiff.a
    DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)
install(FILES "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libtiff.a" TYPE LIB)
ELSE()
set_target_properties(libtiff PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/win64/libtiff.lib")
set_target_properties(libtiff PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

file(COPY bin/win64/libtiff.lib
    DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)
install(FILES "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libtiff.lib" TYPE LIB)
ENDIF()

