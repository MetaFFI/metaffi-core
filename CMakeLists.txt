
# add SDK
set(IS_RUN_SDK_TEST TRUE)
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/plugin-sdk")

# add XLLR
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/XLLR")

# CLI
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/CLI")

# run SDK tests
include("${CMAKE_CURRENT_LIST_DIR}/plugin-sdk/run_sdk_tests.cmake")


add_custom_target(metaffi-core ALL
	DEPENDS ${cdts_test} ${xllr_capi_test} XLLR metaffi
)