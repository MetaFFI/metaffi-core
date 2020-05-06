# target_sources use absolute path
cmake_policy(SET CMP0076 NEW)

FILE(GLOB headers "${CMAKE_CURRENT_LIST_DIR}/*.h")
FILE(GLOB sources "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

add_library(xllr SHARED)

target_include_directories(xllr PRIVATE ${headers})
target_sources(xllr PRIVATE ${sources})


