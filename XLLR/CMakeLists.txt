
#------------------------------- XLLR Project ------------------------------------------
hunter_add_package(Boost COMPONENTS thread filesystem)
add_boost(thread filesystem)


# build XLLR
prepend_dir_to_files(sdk_files_for_xllr "${METAFFI_SDK}/runtime" "cdts_traverse_construct.cpp xllr_capi_loader.c cdt.cpp")
FILE(GLOB sources "${CMAKE_CURRENT_LIST_DIR}/*.cpp" ${sdk_files_for_xllr})
add_library(xllr SHARED)
target_sources(xllr PRIVATE ${sources})
target_include_directories(xllr PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${Boost_INCLUDE_DIRS} ${METAFFI_SDK})
target_link_libraries(xllr PRIVATE ${CMAKE_DL_LIBS} Boost::filesystem Boost::thread)
set_target_properties(xllr PROPERTIES PREFIX "" POSITION_INDEPENDENT_CODE ON)

set_property(TARGET xllr APPEND PROPERTY INSTALL_RPATH "$ORIGIN;$ORIGIN/lib")
#copy_file_from_project_binary_to_metaffi_home(xllr "xllr${CMAKE_SHARED_LIBRARY_SUFFIX}")


#unitest
prepend_dir_to_files(cdts_wrapper_test_sdk_sources "${METAFFI_SDK}/runtime" "cdts_test.cpp cdts_wrapper.cpp cdts_traverse_construct.cpp xllr_static_capi_loader.cpp xllr_capi_loader.c cdt.cpp")
add_executable(cdts_wrapper_test_sdk ${cdts_wrapper_test_sdk_sources})
target_link_libraries(cdts_wrapper_test_sdk PRIVATE Threads::Threads doctest::doctest)
link_to_dl_for_non_windows(cdts_wrapper_test_sdk)
add_test(NAME "CDTS Wrapper" COMMAND cdts_wrapper_test_sdk WORKING_DIRECTORY ${CMAKE_BINARY_DIR})


# install
install_target(xllr .)
