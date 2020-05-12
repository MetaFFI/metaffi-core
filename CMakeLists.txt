cmake_minimum_required(VERSION 3.16.0) # TODO: check what is the lowest
project(openffi VERSION 0.0.1)

# Minimum C++17 - TODO: check if can be lowered
set(CMAKE_CXX_STANDARD 17)

# Build XLLR
include(${CMAKE_CURRENT_SOURCE_DIR}/XLLR/CMakeLists.txt)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
