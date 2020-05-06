cmake_minimum_required(VERSION 3.10)
project(openffi VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

# Build XLLR
include(${CMAKE_CURRENT_SOURCE_DIR}/XLLR/CMakeLists.txt)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
