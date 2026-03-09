# add SDK
if(NOT DEFINED sdk_src)
	message(FATAL_ERROR "sdk_src is not defined. Ensure sdk/CMakeLists.txt is added before metaffi-core.")
endif()

# add XLLR
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/XLLR")

# CLI
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/CLI")

add_custom_target(metaffi-core)
add_dependencies(metaffi-core xllr metaffi cdts_test xllr_capi_test)
set(metaffi-core ${metaffi-core} PARENT_SCOPE)
