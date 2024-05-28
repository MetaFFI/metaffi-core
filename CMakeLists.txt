cmake_minimum_required(VERSION 3.16.3)

# --- Global CMake header for all MetaFFI projects ---

# SDK Paths
set(METAFFI_SDK ${CMAKE_CURRENT_LIST_DIR}/plugin-sdk)
set(CMAKE_SCRIPTS_DIR ${CMAKE_CURRENT_LIST_DIR}/plugin-sdk/cmake)

# add SDK
add_subdirectory(${METAFFI_SDK})

# VCPKG
add_vcpkg()

# policies
add_global_policies()

# set global C/C++ flags and compiler options
c_cpp_global_settings()

# set RPATH
add_rpath()

# --- Root project ---
project(metaffi-core VERSION 0.0.1)

# global debug settings for all MetaFFI projects
c_cpp_debug_settings()

# --- Dependencies ---
add_ctest_and_unitest_libs()
add_boost(thread program_options filesystem)

# --- Add Targets ---
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/XLLR/)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/CLI/)


# --- Tests ---

# SDK unitests (done only in core project)
# unitests
prepend_dir_to_files(expand_env_test_src "${METAFFI_SDK}/utils" "expand_env.cpp expand_env_test.cpp")
add_executable(expand_env_test ${expand_env_test_src})
target_link_libraries(expand_env_test PRIVATE doctest::doctest)
add_test(NAME "Expand Environment Variables" COMMAND expand_env_test)

add_go_test("MetaFFI IDL Definition" WORKING_DIRECTORY ${METAFFI_SDK}/compiler/go/IDL)

# --- Install ---
install_globals()

# install SDK - occurs only in core project
# Copy "include" code to METAFFI_HOME/include
install(FILES   ${METAFFI_SDK}/runtime/metaffi_primitives.h
				${METAFFI_SDK}/runtime/cdt.h
				${METAFFI_SDK}/runtime/xcall.h
				${METAFFI_SDK}/runtime/xllr_capi_loader.h
				${METAFFI_SDK}/runtime/xllr_capi_loader.c
				${METAFFI_SDK}/runtime/cdts_traverse_construct.h
		DESTINATION "include")

# install dependencies
install_boost(filesystem program_options thread)