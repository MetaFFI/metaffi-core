
# build openffi.compiler.python Go dynamic library

# TODO: make c-shared as PIC

FILE(GLOB go_sources "${CMAKE_CURRENT_LIST_DIR}/*.go")

find_program(GOEXEC go)
if(NOT GOEXEC)
	message(WARNING "Go not found - Skipping build of Python support")
else()

# build
add_custom_target(openffi.compiler.python ALL)
add_custom_command(TARGET openffi.compiler.python
					DEPENDS ${go_sources}
					WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
					COMMAND ${GOEXEC} build -buildmode=c-shared -gcflags=-shared -o ${CMAKE_CURRENT_BINARY_DIR}/openffi.compiler.python${CMAKE_SHARED_LIBRARY_SUFFIX} ${GO_SRCS}
					COMMENT "Building openffi.compiler.python compiler plugin")


# Unitest
add_test(NAME compiler_go_gotest
		COMMAND ${GOEXEC} test
		WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

endif()