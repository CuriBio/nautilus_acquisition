add_library(tango_incl INTERFACE)
target_include_directories(tango_incl
    INTERFACE include)

add_library(tango_lib STATIC
    ./src/Tango.cpp)

target_include_directories(tango_lib
    PUBLIC include
    PRIVATE src)

target_link_libraries(tango_lib
    PRIVATE
    project_options
    project_warnings)

IF (WIN32)
#add_library(tango_lib STATIC IMPORTED GLOBAL)
#set_target_properties(tango_lib PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/win64/tango.lib")
#set_target_properties(tango_lib PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")
#set_target_properties(tango_dll PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/win64/Tango_DLL.dll")

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/win64/Tango_DLL.dll DESTINATION ${CMAKE_INSTALL_BINDIR})
ENDIF()
