# This file will be configured to contain variables for CPack. These variables
# should be set in the CMake list file of the project before CPack module is
# included. The list of available CPACK_xxx variables and their associated
# documentation may be obtained using
#  cpack --help-variable-list
#
# Some variables are common to all generators (e.g. CPACK_PACKAGE_NAME)
# and some are specific to a generator
# (e.g. CPACK_NSIS_EXTRA_INSTALL_COMMANDS). The generator specific variables
# usually begin with CPACK_<GENNAME>_xxxx.


set(CPACK_BUILD_SOURCE_DIRS "D:/Users/nautilus/Desktop/nautilus_acquisition;D:/Users/nautilus/Desktop/nautilus_acquisition/build-R")
set(CPACK_CMAKE_GENERATOR "Visual Studio 17 2022")
set(CPACK_COMPONENTS_ALL "Unspecified;nautilai")
set(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "TRUE")
set(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "TRUE")
set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_FILE "C:/Program Files/CMake/share/cmake-3.25/Templates/CPack.GenericDescription.txt")
set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_SUMMARY "nautilai built using CMake")
set(CPACK_DMG_SLA_USE_RESOURCE_FILE_LICENSE "ON")
set(CPACK_GENERATOR "ZIP;NSIS")
set(CPACK_INSTALL_CMAKE_PROJECTS "D:/Users/nautilus/Desktop/nautilus_acquisition/build-R;nautilai;ALL;/")
set(CPACK_INSTALL_PREFIX "C:/Program Files (x86)/nautilus")
set(CPACK_MODULE_PATH "D:/Users/nautilus/.conan/data/toml11/3.7.1/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/;D:/Users/nautilus/.conan/data/cxxopts/3.0.0/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/;D:/Users/nautilus/.conan/data/cxxopts/3.0.0/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/;D:/Users/nautilus/.conan/data/toml11/3.7.1/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/;D:/Users/nautilus/.conan/data/cxxopts/3.0.0/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/;D:/Users/nautilus/.conan/data/toml11/3.7.1/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/;D:/Users/nautilus/.conan/data/cxxopts/3.0.0/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/;D:/Users/nautilus/.conan/data/toml11/3.7.1/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/;D:/Users/nautilus/Desktop/nautilus_acquisition/build-R;D:/Users/nautilus/Desktop/nautilus_acquisition/libs/qt6/6.4.1/msvc2019_64/lib/cmake/Qt6;D:/Users/nautilus/Desktop/nautilus_acquisition/libs/qt6/6.4.1/msvc2019_64/lib/cmake/Qt6/3rdparty/extra-cmake-modules/find-modules;D:/Users/nautilus/Desktop/nautilus_acquisition/libs/qt6/6.4.1/msvc2019_64/lib/cmake/Qt6/3rdparty/kwin")
set(CPACK_NSIS_COMPRESSOR "lzma")
set(CPACK_NSIS_DISPLAY_NAME "Nautilai")
set(CPACK_NSIS_INSTALLED_ICON_NAME "Nautilai.exe")
set(CPACK_NSIS_INSTALLER_ICON_CODE "")
set(CPACK_NSIS_INSTALLER_MUI_ICON_CODE "")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
set(CPACK_NSIS_MENU_LINKS "Nautilai.exe;Nautilai - Curi Bio")
set(CPACK_NSIS_PACKAGE_NAME "Nautilai")
set(CPACK_NSIS_UNINSTALL_NAME "Uninstall")
set(CPACK_OUTPUT_CONFIG_FILE "D:/Users/nautilus/Desktop/nautilus_acquisition/build-R/CPackConfig.cmake")
set(CPACK_PACKAGE_DEFAULT_LOCATION "/")
set(CPACK_PACKAGE_DESCRIPTION_FILE "C:/Program Files/CMake/share/cmake-3.25/Templates/CPack.GenericDescription.txt")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Nautilai Installation")
set(CPACK_PACKAGE_FILE_NAME "Nautilai-0.1.41-win64")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Nautilai")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "Nautilai")
set(CPACK_PACKAGE_NAME "Nautilai")
set(CPACK_PACKAGE_RELOCATABLE "true")
set(CPACK_PACKAGE_VENDOR "Humanity")
set(CPACK_PACKAGE_VERSION "0.1.41")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "41")
set(CPACK_RESOURCE_FILE_LICENSE "D:/Users/nautilus/Desktop/nautilus_acquisition/LICENSE.txt")
set(CPACK_RESOURCE_FILE_README "C:/Program Files/CMake/share/cmake-3.25/Templates/CPack.GenericDescription.txt")
set(CPACK_RESOURCE_FILE_WELCOME "C:/Program Files/CMake/share/cmake-3.25/Templates/CPack.GenericWelcome.txt")
set(CPACK_SET_DESTDIR "OFF")
set(CPACK_SOURCE_GENERATOR "ZIP;TGZ")
set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "D:/Users/nautilus/Desktop/nautilus_acquisition/build-R/CPackSourceConfig.cmake")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "-src")
set(CPACK_SYSTEM_NAME "win64")
set(CPACK_THREADS "1")
set(CPACK_TOPLEVEL_TAG "win64")
set(CPACK_WIX_SIZEOF_VOID_P "8")

if(NOT CPACK_PROPERTIES_FILE)
  set(CPACK_PROPERTIES_FILE "D:/Users/nautilus/Desktop/nautilus_acquisition/build-R/CPackProperties.cmake")
endif()

if(EXISTS ${CPACK_PROPERTIES_FILE})
  include(${CPACK_PROPERTIES_FILE})
endif()