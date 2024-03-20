find_package(Doxygen)
if (DOXYGEN_FOUND)
	# set input and output files
	SET(DOXYGEN_INPUT_DIR             ${PROJECT_SOURCE_DIR}/src)
	SET(DOXYGEN_OUTPUT_DIR            ${PROJECT_SOURCE_DIR}/doc)
	SET(DOXYGEN_CONFIGFILE_TEMPLATE   ${PROJECT_SOURCE_DIR}/doxyfile.in)
	SET(DOXYGEN_CONFIGFILE            ${PROJECT_BINARY_DIR}/doxyfile)
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
