set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS program_options filesystem) # Boost library

include(CTest)
enable_testing()

# target_sources use absolute path
cmake_policy(SET CMP0076 NEW)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

FILE(GLOB sources "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

# build MetaFFI CLI
add_executable(metaffi_cli)
set_target_properties(metaffi_cli PROPERTIES OUTPUT_NAME "metaffi")
target_include_directories(metaffi_cli PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${Boost_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_LIST_DIR})

target_sources(metaffi_cli PUBLIC ${sources})

if(WIN32)
	target_link_directories(metaffi_cli PUBLIC ${BOOST_LIBRARYDIR})
else()
	target_link_libraries(metaffi_cli ${Boost_LIBRARIES})
endif()

if(NOT WIN32)
	target_link_libraries(metaffi_cli dl)
endif()


# install
install(TARGETS metaffi_cli DESTINATION .)

if(${CMAKE_SYSTEM_NAME} MATCHES Linux)
	ADD_DEB_DEPENDS(${CMAKE_BINARY_DIR} "metaffi" "python|boost|expat|jvm" INSTALL_FILES INSTALL_RESOLVED_FILES INSTALL_FILES_ERROR)

	if (${INSTALL_FILES_ERROR})
		message( FATAL_ERROR "Dependencies files resolution failed: " ${INSTALL_FILES_ERROR} )
	endif (${INSTALL_FILES_ERROR})

	if(INSTALL_FILES)
		install(FILES ${INSTALL_FILES} # Copy all libraries and symlinks pointing to real paths
				DESTINATION .)
	endif()

	if(INSTALL_RESOLVED_FILES)
		install(FILES ${INSTALL_RESOLVED_FILES} # Copy all libraries real paths
				DESTINATION .)
	endif()
endif()