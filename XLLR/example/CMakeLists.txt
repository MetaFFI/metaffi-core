
# build xllr.example
add_library(xllr.example SHARED ${CMAKE_CURRENT_LIST_DIR}/example_api.h ${CMAKE_CURRENT_LIST_DIR}/example_api.c)

# remove the "lib" prefix in linux to get the expected filename "xllr.example.[ext]"
SET_TARGET_PROPERTIES(xllr.example PROPERTIES PREFIX "")

# Unitest
add_executable(xllr_test "${CMAKE_CURRENT_LIST_DIR}/xllr_test.cpp")
target_link_libraries(xllr_test xllr)
add_test(NAME test_runtime_success COMMAND xllr_test 1)
add_test(NAME test_runtime_fail COMMAND xllr_test 2)
add_test(NAME test_module_success COMMAND xllr_test 3)
add_test(NAME test_module_free_module_via_free_runtime_success COMMAND xllr_test 4)
add_test(NAME test_module_lazy_runtime_success COMMAND xllr_test 5)
add_test(NAME test_module_module_not_exist_fail COMMAND xllr_test 6)
add_test(NAME test_call_success COMMAND xllr_test 7)
add_test(NAME test_call_fail COMMAND xllr_test 8)