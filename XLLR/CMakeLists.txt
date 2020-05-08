find_package(Boost COMPONENTS thread filesystem REQUIRED) # Boost library

include(CTest)
enable_testing()

# target_sources use absolute path
cmake_policy(SET CMP0076 NEW)

# gather files
FILE(GLOB test_sources "${CMAKE_CURRENT_LIST_DIR}/xllr_test.cpp")

FILE(GLOB sources "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
LIST(REMOVE_ITEM sources "${test_sources}")

# build XLLR
add_library(xllr SHARED)
target_include_directories(xllr PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${Boost_INCLUDE_DIRS})
target_sources(xllr PUBLIC ${sources})
target_link_libraries(xllr Boost::thread Boost::filesystem dl)

# build xllr.example dynamic library
include(${CMAKE_CURRENT_SOURCE_DIR}/XLLR/example/CMakeLists.txt)

# Unitest
add_executable(xllr_test "${CMAKE_CURRENT_LIST_DIR}/xllr_test.cpp")
target_link_libraries(xllr_test xllr)
add_test(NAME xllr_test COMMAND xllr_test)
