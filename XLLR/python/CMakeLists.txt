
# build xllr.python
add_library(xllr.python
			SHARED
			${CMAKE_CURRENT_LIST_DIR}/python_api.h
			${CMAKE_CURRENT_LIST_DIR}/python_api.cpp
			${CMAKE_CURRENT_LIST_DIR}/utils.h
			${CMAKE_CURRENT_LIST_DIR}/utils.cpp
)
target_include_directories(xllr.python PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${Boost_INCLUDE_DIRS} ${Python3_INCLUDE_DIRS})

set_property(TARGET xllr.python PROPERTY POSITION_INDEPENDENT_CODE ON)

# remove the "lib" prefix in linux to get the expected filename "xllr.python.[ext]"
SET_TARGET_PROPERTIES(xllr.python PROPERTIES PREFIX "")

target_link_libraries(xllr.python Python3::Python ${Boost_FILESYSTEM_LIBRARIES})

# Unitest Python
add_executable(xllr_python_test "${CMAKE_CURRENT_LIST_DIR}/xllr_python_test.cpp")
target_link_libraries(xllr_python_test xllr)
add_test(NAME test_python_runtime_success COMMAND xllr_python_test 1)
add_test(NAME test_python_module_success COMMAND xllr_python_test 2)
add_test(NAME test_python_module_free_module_via_free_runtime_success COMMAND xllr_python_test 3)
add_test(NAME test_python_module_lazy_runtime_success COMMAND xllr_python_test 4)
add_test(NAME test_python_module_module_not_exist_fail COMMAND xllr_python_test 5)
add_test(NAME test_python_call_success COMMAND xllr_python_test 6)
add_test(NAME test_python_call_fail COMMAND xllr_python_test 7)