cmake_minimum_required(VERSION 3.16.0)
project(openffi VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)

# Build XLLR
include(${CMAKE_CURRENT_SOURCE_DIR}/XLLR/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/CLI/CMakeLists.txt)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


# System Test
add_executable(system_tests "${CMAKE_CURRENT_LIST_DIR}/system_tests.cpp")
target_link_libraries(system_tests xllr)
add_test(NAME test_python COMMAND system_tests test_python)