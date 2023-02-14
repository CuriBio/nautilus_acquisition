add_library(nidaqmx_incl INTERFACE)
target_include_directories(nidaqmx_incl INTERFACE include)
add_library(nidaqmx STATIC IMPORTED GLOBAL)

IF (WIN32)
    set_target_properties(nidaqmx PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/msvc/nidaqmx.lib")
    set_target_properties(nidaqmx PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")
ENDIF()

