add_library(ffmpeg_incl INTERFACE)
target_include_directories(ffmpeg_incl INTERFACE include)

IF (NOT WIN32)

add_library(libavcodec STATIC IMPORTED GLOBAL)
add_library(libavformat STATIC IMPORTED GLOBAL)
add_library(libavutil STATIC IMPORTED GLOBAL)
add_library(libswresample STATIC IMPORTED GLOBAL)
add_library(libswscale STATIC IMPORTED GLOBAL)

set_target_properties(libavcodec PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/arm64/libavcodec.a")
set_target_properties(libavcodec PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

set_target_properties(libavformat PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/arm64/libavformat.a")
set_target_properties(libavformat PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

set_target_properties(libavutil PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/arm64/libavutil.a")
set_target_properties(libavutil PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

set_target_properties(libswresample PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/arm64/libswresample.a")
set_target_properties(libswresample PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

set_target_properties(libswscale PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/arm64/libswscale.a")
set_target_properties(libswscale PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

ELSE()
message("######################## ADDING FFMPEG DLLs ${CMAKE_CURRENT_LIST_DIR}/bin/win64/avodec.lib")
add_library(libavcodec SHARED IMPORTED GLOBAL)
add_library(libavformat SHARED IMPORTED GLOBAL)
add_library(libavutil SHARED IMPORTED GLOBAL)
add_library(libswresample SHARED IMPORTED GLOBAL)
add_library(libswscale SHARED IMPORTED GLOBAL)

set_target_properties(libavcodec PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/win64/avcodec-60.dll")
set_target_properties(libavcodec PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/bin/win64/avcodec.lib")
set_target_properties(libavcodec PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/win64/avcodec-60.dll DESTINATION ${CMAKE_INSTALL_BINDIR})

set_target_properties(libavformat PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/win64/avformat-60.dll")
set_target_properties(libavformat PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/bin/win64/avformat.lib")
set_target_properties(libavformat PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/win64/avformat-60.dll DESTINATION ${CMAKE_INSTALL_BINDIR})

set_target_properties(libavutil PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/win64/avutil-58.dll")
set_target_properties(libavutil PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/bin/win64/avutil.lib")
set_target_properties(libavutil PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/win64/avutil-58.dll DESTINATION ${CMAKE_INSTALL_BINDIR})

set_target_properties(libswresample PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/win64/swresample-4.dll")
set_target_properties(libswresample PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/bin/win64/swresample.lib")
set_target_properties(libswresample PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/win64/swresample-4.dll DESTINATION ${CMAKE_INSTALL_BINDIR})

set_target_properties(libswscale PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/win64/swscale-7.dll")
set_target_properties(libswscale PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/bin/win64/swscale.lib")
set_target_properties(libswscale PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/win64/swscale-7.dll DESTINATION ${CMAKE_INSTALL_BINDIR})

ENDIF()


