# add SDK
if(NOT DEFINED sdk_src)
	message(FATAL_ERROR "sdk_src is not defined. Ensure sdk/CMakeLists.txt is added before metaffi-core.")
endif()

# add XLLR
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/XLLR")

# CLI
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/CLI")

add_custom_target(metaffi-core
	DEPENDS xllr metaffi cdts_test xllr_capi_test
)
set_target_properties(metaffi-core PROPERTIES EXCLUDE_FROM_ALL TRUE)

set(metaffi-core ${metaffi-core} PARENT_SCOPE)
