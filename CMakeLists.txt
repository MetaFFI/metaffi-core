cmake_minimum_required(VERSION 3.16.3)

project(metaffi-core VERSION 0.0.1)
find_package(Boost REQUIRED COMPONENTS filesystem)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)
set(Boost_NO_WARN_NEW_VERSIONS 1)

# To allow CMake to find boost on windows these environment variables must be set:
if(WIN32)
	set(BOOST_INCLUDEDIR    $ENV{Boost_INCLUDE_DIR})
	set(BOOST_LIBRARYDIR    $ENV{Boost_INCLUDE_DIR}/lib64-msvc-14.3)
	set(BOOST_ROOT          $ENV{Boost_INCLUDE_DIR}/boost)
else()
	set(BOOST_LIBRARYDIR    /usr/lib)
endif()


add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/XLLR/)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/CLI/)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
