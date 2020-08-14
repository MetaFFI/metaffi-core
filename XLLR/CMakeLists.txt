set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS thread filesystem REQUIRED) # Boost library
find_package(Python3 COMPONENTS Development) # Python library

include(CTest)
enable_testing()

# target_sources use absolute path
cmake_policy(SET CMP0076 NEW)

FILE(GLOB sources "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# build XLLR
add_library(xllr SHARED)
target_include_directories(xllr PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${Boost_INCLUDE_DIRS})
set_property(TARGET xllr PROPERTY POSITION_INDEPENDENT_CODE ON)

# remove the "lib" prefix in linux to get the expected filename "xllr.python3.[ext]"
SET_TARGET_PROPERTIES(xllr PROPERTIES PREFIX "")
target_sources(xllr PUBLIC ${sources})

target_link_libraries(xllr Boost::thread Boost::filesystem)
if(NOT WIN32)
	target_link_libraries(xllr pthread dl)
endif()

# build xllr plugins dynamic library
include(${CMAKE_CURRENT_SOURCE_DIR}/XLLR/example/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/XLLR/python3/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/XLLR/go/CMakeLists.txt)
