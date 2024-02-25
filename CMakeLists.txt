cmake_minimum_required(VERSION 3.27)

project(milter-alias)

option(BUILD_DOC "Build documentation" ON)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SYSTEMD REQUIRED IMPORTED_TARGET libsystemd)

find_package(Doxygen)
if (DOXYGEN_FOUND)
	# set input and output files
	SET(DOXYGEN_INPUT_DIR             ${PROJECT_SOURCE_DIR}/src)
	SET(DOXYGEN_OUTPUT_DIR            ${PROJECT_SOURCE_DIR}/doc)
	SET(DOXYGEN_CONFIGFILE_TEMPLATE   doxyfile.in)
	SET(DOXYGEN_CONFIGFILE            doxyfile)
	message(DOXYGEN_INPUT_DIR =           "${DOXYGEN_INPUT_DIR}")
	message(DOXYGEN_OUTPUT_DIR =          "${DOXYGEN_OUTPUT_DIR}")
	message(DOXYGEN_CONFIGFILE_TEMPLATE = "${DOXYGEN_CONFIGFILE_TEMPLATE}")
	message(DOXYGEN_CONFIGFILE =          "${DOXYGEN_CONFIGFILE}")

	# create final config file from template (replace CMAKE variables)
	configure_file(${DOXYGEN_CONFIGFILE_TEMPLATE} ${DOXYGEN_CONFIGFILE} @ONLY)

	# Create Output directory.
	FILE(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

	# note the option ALL which allows to build the docs together with the application
	add_custom_target( doc ALL
		COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIGFILE}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen"
		VERBATIM )
else (DOXYGEN_FOUND)
	message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

add_executable(
	milter-alias
	src/daemon.c
	src/extldap.c
	src/extstring.c
	src/ini_parser.c
	src/log.c
	src/main.c
	src/priv_data.c
	src/runtime_setting.c
	src/service_manager.c
	src/string_array.c
	src/smfi.c
	src/smfi_cb.c
)

target_compile_options(milter-alias PRIVATE -Wall -Wextra -pedantic -Werror)
target_compile_features(milter-alias PRIVATE c_std_11)
target_link_libraries(milter-alias PRIVATE milter Threads::Threads PkgConfig::SYSTEMD ldap lber)

install(TARGETS milter-alias RUNTIME DESTINATION bin)
