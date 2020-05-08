
# build xllr.example
add_library(xllr.example SHARED ${CMAKE_CURRENT_LIST_DIR}/example_api.h ${CMAKE_CURRENT_LIST_DIR}/example_api.c)

# remove the "lib" prefix in linux to get the expected filename "xllr.example.[ext]"
SET_TARGET_PROPERTIES(xllr.example PROPERTIES PREFIX "")