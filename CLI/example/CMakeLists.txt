
# build xllr.example
add_library(openffi.compiler.example SHARED ${CMAKE_CURRENT_LIST_DIR}/example_api.h ${CMAKE_CURRENT_LIST_DIR}/example_api.cpp)
set_property(TARGET openffi.compiler.example PROPERTY POSITION_INDEPENDENT_CODE ON)

# remove the "lib" prefix in linux to get the expected filename "xllr.example.[ext]"
SET_TARGET_PROPERTIES(openffi.compiler.example PROPERTIES PREFIX "")

# Unitest
#add_executable(openffi_compiler_example_test "${CMAKE_CURRENT_LIST_DIR}/compiler_example_test.cpp")
#target_link_libraries(openffi_compiler_example_test openffi.compiler.example)
#add_test(NAME compile_to_guest COMMAND openffi_compiler_example_test 1)
#add_test(NAME compile_from_host COMMAND openffi_compiler_example_test 2)