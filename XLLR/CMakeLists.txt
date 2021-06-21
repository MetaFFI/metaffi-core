find_package(Boost REQUIRED COMPONENTS thread filesystem) # Boost library

include(CTest)
enable_testing()

# target_sources use absolute path
cmake_policy(SET CMP0076 NEW)

FILE(GLOB sources "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# build XLLR
add_library(xllr SHARED)
target_include_directories(xllr PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${Boost_INCLUDE_DIRS} $ENV{OPENFFI_HOME})
set_property(TARGET xllr PROPERTY POSITION_INDEPENDENT_CODE ON)

SET_TARGET_PROPERTIES(xllr PROPERTIES PREFIX "")
target_sources(xllr PUBLIC ${sources})

target_link_libraries(xllr Boost::thread Boost::filesystem)
if(NOT WIN32)
	target_link_libraries(xllr pthread dl)
endif()

# Copy openffi_primitives.h to openffi directory
add_custom_command(TARGET xllr POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy
		"${CMAKE_CURRENT_LIST_DIR}/../../plugin-sdk/runtime/openffi_primitives.h"
		"$ENV{OPENFFI_HOME}/include/openffi_primitives.h"
		COMMENT "Copy ${CMAKE_CURRENT_LIST_DIR}/../../plugin-sdk/runtime/openffi_primitives.h -> $ENV{OPENFFI_HOME}/include/openffi_primitives.h")

add_custom_command(TARGET xllr POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy
		"../../plugin-sdk/runtime/common_data_type_helper_loader.h"
		"$ENV{OPENFFI_HOME}/include/common_data_type_helper_loader.h"
		COMMENT "Copy ${CMAKE_CURRENT_LIST_DIR}/../../plugin-sdk/runtime/common_data_type_helper_loader.h -> $ENV{OPENFFI_HOME}/include/common_data_type_helper_loader.h")

add_custom_command(TARGET xllr POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy
		"../../plugin-sdk/runtime/common_data_type_helper_loader.c"
		"$ENV{OPENFFI_HOME}/include/common_data_type_helper_loader.c"
		COMMENT "Copy ${CMAKE_CURRENT_LIST_DIR}/../../plugin-sdk/runtime/common_data_type_helper_loader.c -> $ENV{OPENFFI_HOME}/include/common_data_type_helper_loader.c")