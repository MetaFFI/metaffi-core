find_package(Boost REQUIRED COMPONENTS thread filesystem) # Boost library
find_package(LLVM REQUIRED CONFIG)

include(CTest)
enable_testing()

# target_sources use absolute path
cmake_policy(SET CMP0076 NEW)

FILE(GLOB sources "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# build XLLR
add_library(xllr SHARED)
target_include_directories(xllr PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${Boost_INCLUDE_DIRS} $ENV{METAFFI_HOME} ${LLVM_INCLUDE_DIRS})

set_property(TARGET xllr PROPERTY POSITION_INDEPENDENT_CODE ON)

SET_TARGET_PROPERTIES(xllr PROPERTIES PREFIX "")
target_sources(xllr PUBLIC ${sources})

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_compile_options(${LLVM_DEFINITIONS_LIST})
llvm_map_components_to_libnames(llvm_libs core orcjit support nativecodegen)

if(NOT WIN32)
	target_link_libraries(xllr Boost::thread Boost::filesystem pthread dl Boost::dynamic_linking ${llvm_libs})
	target_link_directories(xllr PUBLIC ${BOOST_LIBRARYDIR})
else()
	target_link_libraries(xllr Boost::thread Boost::filesystem Boost::dynamic_linking ${llvm_libs})
endif()

# Copy metaffi_primitives.h to metaffi directory
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

add_executable(xcall_jit_test ${CMAKE_CURRENT_LIST_DIR}/xcall_jit_test.cpp ${CMAKE_CURRENT_LIST_DIR}/xcall_jit.cpp)
target_include_directories(xcall_jit_test PUBLIC ${LLVM_INCLUDE_DIRS})

if(NOT WIN32)
	target_link_libraries(xcall_jit_test dl ${llvm_libs})
endif()

add_test(NAME xcall_jit_test COMMAND xcall_jit_test)