cmake_minimum_required(VERSION 3.17.3)
project(openffi VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)

# To allow CMake to find boost on windows these environment variables must be set:
# BOOST_INCLUDEDIR    [path to boost]
# BOOST_LIBRARYDIR    [path to boost]\lib64-msvc-[version]
# BOOST_ROOT          [path to boost]\boost

# Build XLLR
include(${CMAKE_CURRENT_SOURCE_DIR}/XLLR/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/CLI/CMakeLists.txt)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
