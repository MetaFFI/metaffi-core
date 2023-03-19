
#------------------------------- metaffi-cli Project ------------------------------------------
hunter_add_package(Boost COMPONENTS program_options filesystem)
add_boost(program_options filesystem)


# build MetaFFI CLI
FILE(GLOB metaffi_cli_sources "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_executable(metaffi_cli)
set_target_properties(metaffi_cli PROPERTIES OUTPUT_NAME "metaffi")
target_include_directories(metaffi_cli PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${Boost_INCLUDE_DIRS} ${METAFFI_SDK})
target_sources(metaffi_cli PRIVATE ${metaffi_cli_sources} ${METAFFI_SDK}/utils/expand_env.cpp)
target_link_libraries(metaffi_cli PRIVATE ${CMAKE_DL_LIBS} Boost::filesystem Boost::program_options)
link_to_dl_for_non_windows(metaffi_cli)

set_property(TARGET metaffi_cli APPEND PROPERTY BUILD_RPATH "$ORIGIN;$ORIGIN/lib")

#copy_file_from_project_binary_to_metaffi_home(metaffi_cli metaffi${CMAKE_EXECUTABLE_SUFFIX})

# install
install_target(metaffi_cli .)
