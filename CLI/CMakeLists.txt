set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS program_options filesystem REQUIRED) # Boost library

include(CTest)
enable_testing()

# target_sources use absolute path
cmake_policy(SET CMP0076 NEW)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

FILE(GLOB sources "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

# build OpenFFI CLI
add_executable(openffi_cli)
set_target_properties(openffi_cli PROPERTIES OUTPUT_NAME "openffi")
target_include_directories(openffi_cli PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${Boost_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_LIST_DIR})

target_sources(openffi_cli PUBLIC ${sources})

target_link_libraries(openffi_cli Boost::program_options Boost::filesystem)

if(NOT WIN32)
	target_link_libraries(openffi_cli dl)
endif()

# build compiler plugins dynamic library
include(${CMAKE_CURRENT_SOURCE_DIR}/CLI/example/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/CLI/python/CMakeLists.txt)
