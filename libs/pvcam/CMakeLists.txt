add_library(pvcam_incl INTERFACE)
target_include_directories(pvcam_incl
    INTERFACE include)

add_library(pvcam64 STATIC IMPORTED GLOBAL)
set_target_properties(pvcam64 PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/amd64/libpvcam64.lib")
set_target_properties(pvcam64 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

file(COPY bin/amd64/libpvcam64.lib
    DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)

install(FILES "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libpvcam64.lib" TYPE LIB)
