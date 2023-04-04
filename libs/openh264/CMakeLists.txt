add_library(openh264 STATIC IMPORTED GLOBAL)

IF (NOT WIN32)
set_target_properties(openh264 PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/arm64/libopenh264.a")
ELSE()
set_target_properties(openh264 PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/win64/libopenh264.lib")
ENDIF()

