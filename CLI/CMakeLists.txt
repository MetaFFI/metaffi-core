find_package(Boost COMPONENTS program_options filesystem REQUIRED) # Boost library

include(CTest)
enable_testing()

# target_sources use absolute path
cmake_policy(SET CMP0076 NEW)

FILE(GLOB sources "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

# build XLLR
add_executable(openffi_cli)
set_target_properties(openffi_cli PROPERTIES OUTPUT_NAME "openffi")
target_include_directories(openffi_cli PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${Boost_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_LIST_DIR})

target_sources(openffi_cli PUBLIC ${sources})

target_link_libraries(openffi_cli ${Boost_LIBRARIES} dl)

# build xllr plugins dynamic library
#include(${CMAKE_CURRENT_SOURCE_DIR}/XLLR/example/CMakeLists.txt)
