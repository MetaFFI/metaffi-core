# load 3rd parties packages
find_or_install_package(Boost COMPONENTS filesystem thread)

collect_c_cpp_files("${CMAKE_CURRENT_LIST_DIR}"
                    xllr)

c_cpp_shared_lib(xllr
  "${xllr_src};${sdk_src}"
  "${sdk_include_dir};${Boost_INCLUDE_DIRS}"
  "Boost::filesystem;Boost::thread"
  ".")

set(xllr xllr PARENT_SCOPE)

# write a post build script that copies to $ENV{METAFFI_HOME}/include the following files:
# ${sdk_include_dir}/runtime/cdt.h, cdts_traverse_construct.h, metaffi_primitives.h, xcall.h, xllr_capi_loader.c, xllr_capi_loader.h

set(FILES_TO_COPY "")
list(APPEND FILES_TO_COPY "${sdk_include_dir}/runtime/cdt.h")
list(APPEND FILES_TO_COPY "${sdk_include_dir}/runtime/cdts_traverse_construct.h")
list(APPEND FILES_TO_COPY "${sdk_include_dir}/runtime/metaffi_primitives.h")
list(APPEND FILES_TO_COPY "${sdk_include_dir}/runtime/xcall.h")
list(APPEND FILES_TO_COPY "${sdk_include_dir}/runtime/xllr_capi_loader.c")
list(APPEND FILES_TO_COPY "${sdk_include_dir}/runtime/xllr_capi_loader.h")

# make sure target dir exists. don't fail if its already there
file(MAKE_DIRECTORY $ENV{METAFFI_HOME}/include)

foreach(file ${FILES_TO_COPY})
 get_filename_component(FILE_NAME ${file} NAME)
 add_custom_command(TARGET xllr POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy ${file} $ENV{METAFFI_HOME}/include/${FILE_NAME}
 )
endforeach()