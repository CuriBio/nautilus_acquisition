#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "AWS::aws-c-common" for configuration "Debug"
set_property(TARGET AWS::aws-c-common APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(AWS::aws-c-common PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/aws-c-common.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/aws-c-common.dll"
  )

list(APPEND _cmake_import_check_targets AWS::aws-c-common )
list(APPEND _cmake_import_check_files_for_AWS::aws-c-common "${_IMPORT_PREFIX}/lib/aws-c-common.lib" "${_IMPORT_PREFIX}/bin/aws-c-common.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
