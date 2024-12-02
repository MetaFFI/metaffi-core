
find_or_install_package(Boost COMPONENTS filesystem program_options)

collect_c_cpp_files(${CMAKE_CURRENT_LIST_DIR} cli)

c_cpp_exe(metaffi
		"${cli_src};${sdk_src}"
		"${cli_include_dir};${sdk_include_dir};${Boost_INCLUDE_DIRS}"
		"Boost::filesystem;Boost::program_options"
		".")

set(metaffi metaffi PARENT_SCOPE)

