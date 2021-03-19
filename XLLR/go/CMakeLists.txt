find_package(Boost REQUIRED COMPONENTS thread filesystem) # Boost library

include(CTest)
enable_testing()

# build xllr.go
add_library(xllr.go
			SHARED
			${CMAKE_CURRENT_LIST_DIR}/go_api.h
			${CMAKE_CURRENT_LIST_DIR}/go_api.cpp
			${CMAKE_CURRENT_LIST_DIR}/modules_repository.h
			${CMAKE_CURRENT_LIST_DIR}/modules_repository.cpp
)
target_include_directories(xllr.go PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${Boost_INCLUDE_DIRS})

set_property(TARGET xllr.go PROPERTY POSITION_INDEPENDENT_CODE ON)

# remove the "lib" prefix in linux to get the expected filename "xllr.go.[ext]"
SET_TARGET_PROPERTIES(xllr.go PROPERTIES PREFIX "")

target_link_libraries(xllr.go Boost::filesystem)

# Unitest go
add_executable(xllr_go_test "${CMAKE_CURRENT_LIST_DIR}/xllr_go_test.cpp")
target_link_libraries(xllr_go_test xllr)
add_test(NAME test_go_module_success COMMAND xllr_go_test 1)
add_test(NAME test_go_module_free_module_via_free_runtime_success COMMAND xllr_go_test 2)
add_test(NAME test_go_module_lazy_runtime_success COMMAND xllr_go_test 3)
add_test(NAME test_go_module_module_not_exist_fail COMMAND xllr_go_test 4)
add_test(NAME test_go_call_success COMMAND xllr_go_test 5)
add_test(NAME test_go_call_fail COMMAND xllr_go_test 6)