cmake_minimum_required(VERSION 3.16.3)

# policies
cmake_policy(SET CMP0110 NEW) # Allow arbitrary names in CTest names

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_STANDARD 20)
if (CMAKE_BUILD_TYPE MATCHES Debug)
	add_definitions(-DEBUG) # add -DEBUG to "Debug" builds
	set(CMAKE_DEBUG_POSTFIX "")
endif()

# Add SDK
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/plugin-sdk/)
set(METAFFI_SDK ${CMAKE_CURRENT_LIST_DIR}/plugin-sdk)
set(CMAKE_SCRIPTS_DIR ${CMAKE_CURRENT_LIST_DIR}/plugin-sdk/cmake)

# Load hunter package manager
load_hunter_pm(${CMAKE_SCRIPTS_DIR}) # load package manager

# include scripts required for project
include(${CMAKE_SCRIPTS_DIR}/InstallUtils.cmake)

#-----------------------------------------------------------------------------------------------

project(metaffi-core VERSION 0.0.1)

# Add required packages
hunter_add_package(Catch)
add_ctest_and_catch2_packages()

# set RPATH
set(CMAKE_MACOSX_RPATH 1) # tell MacOS RPATH is in use
list(APPEND CMAKE_INSTALL_RPATH "$ORIGIN")

# target directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# add targets
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/XLLR/)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/CLI/)

# SDK unitests (done only in core project)
# unitests
prepend_dir_to_files(expand_env_test_src "${METAFFI_SDK}/utils" "expand_env.cpp expand_env_test.cpp")
add_executable(expand_env_test ${expand_env_test_src})
target_link_libraries(expand_env_test PRIVATE Catch2::Catch2)
add_test(NAME "Expand Environment Variables" COMMAND expand_env_test)

add_go_test("MetaFFI IDL Definition" WORKING_DIRECTORY ${METAFFI_SDK}/compiler/go/IDL)

install(CODE "set(ENV{METAFFI_HOME}, ${CMAKE_BINARY_DIR})")

# cmake install directory (not cpack)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
set(CMAKE_SKIP_INSTALL_RPATH OFF)

#-----------------------------------------------------------------------------------------------

# build installer
set(CPACK_PACKAGE_NAME "MetaFFI")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_COMPONENTS_GROUPING ONE_PER_GROUP)

set(CPACK_PACKAGE_NAME ${PROJECT_NAME} CACHE STRING "MetaFFI")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "metaffi")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/MetaFFI/")

set(CPACK_PACKAGE_INSTALL_DIRECTORY "metaffi")

if(${CMAKE_SYSTEM_NAME} MATCHES Linux)
	set(CPACK_GENERATOR "DEB")
	set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
	set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/metaffi/")
	set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Tsvi Cherny-Shahar")

	set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_LIST_DIR}/installer/postinst;${CMAKE_CURRENT_LIST_DIR}/installer/prerm")

elseif(WIN32)

	set(CPACK_GENERATOR "WIX")
	set(CPACK_WIX_PRODUCT_GUID "07862856-71e8-4c92-88ea-8eb759b591b0")
	set(CPACK_WIX_UPGRADE_GUID "193774b5-8b4d-4684-9539-241f04645802")
	set(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "https://github.com/MetaFFI/")
	set(CPACK_WIX_PATCH_FILE "${CMAKE_CURRENT_LIST_DIR}/installer/WixExtraCommands.xml")

endif()

set(CPACK_RESOURCE_FILE_LICENSE    ${CMAKE_CURRENT_LIST_DIR}/LICENSE.txt)
set(CPACK_RESOURCE_FILE_README     ${CMAKE_CURRENT_LIST_DIR}/README.md)

include(CPack)