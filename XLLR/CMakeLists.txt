find_package(Boost REQUIRED COMPONENTS thread filesystem) # Boost library

include(CTest)
enable_testing()

# target_sources use absolute path
cmake_policy(SET CMP0076 NEW)

FILE(GLOB sources "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# build XLLR
add_library(xllr SHARED)
target_include_directories(xllr PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${Boost_INCLUDE_DIRS} $ENV{METAFFI_HOME})

set_property(TARGET xllr PROPERTY POSITION_INDEPENDENT_CODE ON)

SET_TARGET_PROPERTIES(xllr PROPERTIES PREFIX "")
target_sources(xllr PUBLIC ${sources})

if(NOT WIN32)
	target_link_libraries(xllr Boost::thread Boost::filesystem pthread dl Boost::dynamic_linking)
	target_link_directories(xllr PUBLIC ${BOOST_LIBRARYDIR})
else()
	target_link_libraries(xllr Boost::thread Boost::filesystem Boost::dynamic_linking)
endif()

# Copy "include" code to METAFFI_HOME
add_custom_command(TARGET xllr POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy
		"${CMAKE_CURRENT_LIST_DIR}/../../plugin-sdk/runtime/metaffi_primitives.h"
		"$ENV{METAFFI_HOME}/include/metaffi_primitives.h"
		COMMENT "Copy ${CMAKE_CURRENT_LIST_DIR}/../../plugin-sdk/runtime/metaffi_primitives.h -> $ENV{METAFFI_HOME}/include/metaffi_primitives.h")

add_custom_command(TARGET xllr POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy
		"${CMAKE_CURRENT_LIST_DIR}/../../plugin-sdk/runtime/cdt_structs.h"
		"$ENV{METAFFI_HOME}/include/cdt_structs.h"
		COMMENT "Copy ${CMAKE_CURRENT_LIST_DIR}/../../plugin-sdk/runtime/cdt_structs.h -> $ENV{METAFFI_HOME}/include/cdt_structs.h")

add_custom_command(TARGET xllr POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy
		"${CMAKE_CURRENT_LIST_DIR}/../../plugin-sdk/runtime/cdt_capi_loader.h"
		"$ENV{METAFFI_HOME}/include/cdt_capi_loader.h"
		COMMENT "Copy ${CMAKE_CURRENT_LIST_DIR}/../../plugin-sdk/runtime/cdt_capi_loader.h -> $ENV{METAFFI_HOME}/include/cdt_capi_loader.h")

add_custom_command(TARGET xllr POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy
		"${CMAKE_CURRENT_LIST_DIR}/../../plugin-sdk/runtime/cdt_capi_loader.c"
		"$ENV{METAFFI_HOME}/include/cdt_capi_loader.c"
		COMMENT "Copy ${CMAKE_CURRENT_LIST_DIR}/../../plugin-sdk/runtime/cdt_capi_loader.c -> $ENV{METAFFI_HOME}/include/cdt_capi_loader.c")

add_custom_command(TARGET xllr POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy
		"${CMAKE_CURRENT_BINARY_DIR}/xllr.so"
		"$ENV{METAFFI_HOME}/xllr.so"
		COMMENT "Copy ${CMAKE_CURRENT_BINARY_DIR}/xllr.so -> $ENV{METAFFI_HOME}/xllr.so")

# install
install(TARGETS xllr DESTINATION .)

install(FILES   ${CMAKE_CURRENT_LIST_DIR}/../../plugin-sdk/runtime/metaffi_primitives.h
				${CMAKE_CURRENT_LIST_DIR}/../../plugin-sdk/runtime/cdt_structs.h
				${CMAKE_CURRENT_LIST_DIR}/../../plugin-sdk/runtime/cdt_capi_loader.h
				${CMAKE_CURRENT_LIST_DIR}/../../plugin-sdk/runtime/cdt_capi_loader.c
		DESTINATION "include")

if(${CMAKE_SYSTEM_NAME} MATCHES Linux)
	ADD_DEB_DEPENDS(${CMAKE_BINARY_DIR} "xllr.so" "python|boost|expat|jvm" INSTALL_FILES INSTALL_RESOLVED_FILES INSTALL_FILES_ERROR)
	if (${INSTALL_FILES_ERROR})
		message( FATAL_ERROR "Dependencies files resolution failed: " ${INSTALL_FILES_ERROR} )
	endif (${INSTALL_FILES_ERROR})

	if(INSTALL_FILES)
		install(FILES ${INSTALL_FILES} # Copy all libraries and symlinks pointing to real paths
				DESTINATION .)
	endif()

	if(INSTALL_RESOLVED_FILES)
		install(FILES ${INSTALL_RESOLVED_FILES} # Copy all libraries real paths
				DESTINATION .)
	endif()
endif()