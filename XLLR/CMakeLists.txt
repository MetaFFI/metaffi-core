find_package(Boost COMPONENTS thread filesystem REQUIRED) # Boost library

include(CTest)
enable_testing()

# target_sources use absolute path
cmake_policy(SET CMP0076 NEW)

FILE(GLOB sources "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

# build XLLR
add_library(xllr SHARED)
target_include_directories(xllr PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${Boost_INCLUDE_DIRS})

target_sources(xllr PUBLIC ${sources})
target_link_libraries(xllr Boost::thread Boost::filesystem dl)

# build xllr plugins dynamic library
include(${CMAKE_CURRENT_SOURCE_DIR}/XLLR/example/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/XLLR/python/CMakeLists.txt)
