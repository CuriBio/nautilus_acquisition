# Set the minimum required version of CMake for this project.
cmake_minimum_required(VERSION 3.13)

add_library(awssdk_incl INTERFACE)
target_include_directories(awssdk_incl INTERFACE include)

IF (WIN32)
message("### ADDING AWS SDK DLLs ###")

add_library(libawscore SHARED IMPORTED GLOBAL)
add_library(libawscores3 SHARED IMPORTED GLOBAL)
add_library(libawscrt SHARED IMPORTED GLOBAL)
add_library(libawss3 SHARED IMPORTED GLOBAL)
add_library(libawscal SHARED IMPORTED GLOBAL)
add_library(libawscompression SHARED IMPORTED GLOBAL)
add_library(libawshttp SHARED IMPORTED GLOBAL)
add_library(libawsio SHARED IMPORTED GLOBAL)
add_library(libawsmqtt SHARED IMPORTED GLOBAL)
add_library(libawsutils SHARED IMPORTED GLOBAL)
add_library(libawschecksums SHARED IMPORTED GLOBAL)
add_library(libawscommon SHARED IMPORTED GLOBAL)
add_library(libawseventstream SHARED IMPORTED GLOBAL)
add_library(libawsauth SHARED IMPORTED GLOBAL)

set_target_properties(libawscore PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/aws-cpp-sdk-core.dll")
set_target_properties(libawscore PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/aws-cpp-sdk-core.lib")
set_target_properties(libawscore PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/aws-cpp-sdk-core.dll DESTINATION ${CMAKE_INSTALL_BINDIR})

# set_target_properties(libawscores3 PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/aws-cpp-sdk-s3.dll")
# set_target_properties(libawscores3 PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/aws-cpp-sdk-s3.lib")
# set_target_properties(libawscores3 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

set_target_properties(libawscrt PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/aws-crt-cpp.dll")
set_target_properties(libawscrt PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/aws-crt-cpp.lib")
set_target_properties(libawscrt PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

set_target_properties(libawss3 PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/aws-c-s3.dll")
set_target_properties(libawss3 PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/aws-c-s3.lib")
set_target_properties(libawss3 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

set_target_properties(libawsauth PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/aws-c-auth.dll")
set_target_properties(libawsauth PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/aws-c-auth.lib")
set_target_properties(libawsauth PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

set_target_properties(libawscommon PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/aws-c-common.dll")
set_target_properties(libawscommon PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/aws-c-common.lib")
set_target_properties(libawscommon PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

set_target_properties(libawshttp PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/aws-c-http.dll")
set_target_properties(libawshttp PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/aws-c-http.lib")
set_target_properties(libawshttp PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

set_target_properties(libawsio PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/aws-c-io.dll")
set_target_properties(libawsio PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/aws-c-io.lib")
set_target_properties(libawsio PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

set_target_properties(libawscompression PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/aws-c-compression.dll")
set_target_properties(libawscompression PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/aws-c-compression.lib")
set_target_properties(libawscompression PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

set_target_properties(libawschecksums PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/aws-checksums.dll")
set_target_properties(libawschecksums PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/aws-checksums.lib")
set_target_properties(libawschecksums PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

set_target_properties(libawsmqtt PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/aws-c-mqtt.dll")
set_target_properties(libawsmqtt PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/aws-c-mqtt.lib")
set_target_properties(libawsmqtt PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

set_target_properties(libawsutils PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/aws-c-sdkutils.dll")
set_target_properties(libawsutils PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/aws-c-sdkutils.lib")
set_target_properties(libawsutils PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

set_target_properties(libawseventstream PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/aws-c-event-stream.dll")
set_target_properties(libawseventstream PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/aws-c-event-stream.lib")
set_target_properties(libawseventstream PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

set_target_properties(libawscal PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/aws-c-cal.dll")
set_target_properties(libawscal PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/aws-c-cal.lib")
set_target_properties(libawscal PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

# install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/aws-cpp-sdk-s3.dll DESTINATION ${CMAKE_INSTALL_BINDIR})
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