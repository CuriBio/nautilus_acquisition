#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "AWS::aws-c-sdkutils" for configuration "Debug"
set_property(TARGET AWS::aws-c-sdkutils APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(AWS::aws-c-sdkutils PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/aws-c-sdkutils.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/aws-c-sdkutils.dll"
  )

list(APPEND _cmake_import_check_targets AWS::aws-c-sdkutils )
list(APPEND _cmake_import_check_files_for_AWS::aws-c-sdkutils "${_IMPORT_PREFIX}/lib/aws-c-sdkutils.lib" "${_IMPORT_PREFIX}/bin/aws-c-sdkutils.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
