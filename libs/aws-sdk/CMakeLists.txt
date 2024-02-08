# Set the minimum required version of CMake for this project.
cmake_minimum_required(VERSION 3.13)

add_library(awssdk_incl INTERFACE)
target_include_directories(awssdk_incl INTERFACE include)

IF (WIN32)
message("### ADDING AWS SDK DLLs ###")

add_library(libawscore SHARED IMPORTED GLOBAL)
add_library(libawscores3 SHARED IMPORTED GLOBAL)

set_target_properties(libawscore PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/aws-cpp-sdk-core.dll")
set_target_properties(libawscore PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/aws-cpp-sdk-core.lib")
set_target_properties(libawscore PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/aws-cpp-sdk-core.dll DESTINATION ${CMAKE_INSTALL_BINDIR})

set_target_properties(libawscores3 PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/aws-cpp-sdk-s3.dll")
set_target_properties(libawscores3 PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/aws-cpp-sdk-s3.lib")
set_target_properties(libawscores3 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/aws-cpp-sdk-s3.dll DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/aws-c-auth.dll DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/aws-c-compression.dll DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/aws-c-http.dll DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/aws-c-io.dll DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/aws-c-mqtt.dll DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/aws-c-s3.dll DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/aws-c-sdkutils.dll DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/aws-crt-cpp.dll DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/aws-c-common.dll DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/aws-c-event-stream.dll DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/aws-checksums.dll DESTINATION ${CMAKE_INSTALL_BINDIR})

ENDIF()