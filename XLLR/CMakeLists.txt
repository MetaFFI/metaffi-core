
#------------------------------- XLLR ------------------------------------------
# XLLR is a library that provides a C API for the CDTS library. It is a shared library that
# can be loaded by a host application to use the CDTS library. The library is built using
# the CDTS library and the Metaffi SDK. The library is built as a shared library and is
# installed in the Metaffi home directory.

# --- XLLR dynamic library ---
prepend_dir_to_files(sdk_files_for_xllr "${METAFFI_SDK}/runtime" "cdts_traverse_construct.cpp xllr_capi_loader.c cdt.cpp")
FILE(GLOB sources "${CMAKE_CURRENT_LIST_DIR}/*.cpp" ${sdk_files_for_xllr})
add_library(xllr SHARED)
target_sources(xllr PRIVATE ${sources})
target_include_directories(xllr PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${METAFFI_SDK})
target_link_libraries(xllr PRIVATE ${CMAKE_DL_LIBS} Boost::filesystem Boost::thread)
set_target_properties(xllr PROPERTIES PREFIX "" POSITION_INDEPENDENT_CODE ON)

set_property(TARGET xllr APPEND PROPERTY INSTALL_RPATH "$ORIGIN;$ORIGIN/lib")


# --- Tests ---
prepend_dir_to_files(cdts_wrapper_test_sdk_sources "${METAFFI_SDK}/runtime" "cdts_test.cpp cdts_wrapper.cpp cdts_traverse_construct.cpp xllr_static_capi_loader.cpp xllr_capi_loader.c cdt.cpp")
add_executable(cdts_wrapper_test_sdk ${cdts_wrapper_test_sdk_sources})
target_link_libraries(cdts_wrapper_test_sdk PRIVATE ${CMAKE_DL_LIBS} Threads::Threads doctest::doctest)
add_test(NAME "CDTS Wrapper" COMMAND cdts_wrapper_test_sdk WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

prepend_dir_to_files(xllr_capi_test_sdk_sources "${METAFFI_SDK}/runtime" "xllr_capi_test.cpp cdts_wrapper.cpp xllr_static_capi_loader.cpp xllr_capi_loader.c cdt.cpp")
add_executable(xllr_capi_test_sdk_sources ${xllr_capi_test_sdk_sources})
target_link_libraries(xllr_capi_test_sdk_sources PRIVATE ${CMAKE_DL_LIBS} Threads::Threads doctest::doctest)
add_test(NAME "XLLR C-API" COMMAND xllr_capi_test_sdk_sources WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

# --- Install ---
install_target(xllr .)
