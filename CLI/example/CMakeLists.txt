
# build openffi.compiler.example
add_library(openffi.compiler.example SHARED ${CMAKE_CURRENT_LIST_DIR}/example_api.h ${CMAKE_CURRENT_LIST_DIR}/example_api.cpp)
set_property(TARGET openffi.compiler.example PROPERTY POSITION_INDEPENDENT_CODE ON)

# remove the "lib" prefix in linux to get the expected filename "xllr.example.[ext]"
SET_TARGET_PROPERTIES(openffi.compiler.example PROPERTIES PREFIX "")

# Unitest
add_test(NAME compile_example COMMAND openffi -c --idl myidl -t example -f example)
add_test(NAME compile_missing_langs COMMAND openffi -c --idl myidl)
add_test(NAME compile_missing_idl COMMAND openffi -c -t example -f example)

## set negative tests
set_tests_properties(compile_missing_langs compile_missing_idl PROPERTIES WILL_FAIL 1)