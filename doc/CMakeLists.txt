find_package(Doxygen)
if(DOXYGEN_FOUND)

	find_program(EXEC_QHELPGENERATOR qhelpgenerator)

	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(doc
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM
	)
endif(DOXYGEN_FOUND)

find_package(Sphinx)
if(SPHINX_FOUND)

	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/conf.py ${CMAKE_CURRENT_BINARY_DIR}/build/conf.py @ONLY)
	add_custom_target(manual
		${SPHINX_EXECUTABLE}
		-a -q -b html
		-c ${CMAKE_CURRENT_BINARY_DIR}/build
		-d ${CMAKE_CURRENT_BINARY_DIR}/doctrees
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}/manual
		COMMENT "Building HTML documentation with Sphinx"
	)
	add_custom_target(manual-dir
		${SPHINX_EXECUTABLE}
		-a -q -b dirhtml
		-c ${CMAKE_CURRENT_BINARY_DIR}/build
		-d ${CMAKE_CURRENT_BINARY_DIR}/doctrees
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}/manual
		COMMENT "Building HTML documentation with Sphinx"
	)
	add_custom_target(manual-single
		${SPHINX_EXECUTABLE}
		-a -q -b singlehtml
		-c ${CMAKE_CURRENT_BINARY_DIR}/build
		-d ${CMAKE_CURRENT_BINARY_DIR}/doctrees
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}/manual
		COMMENT "Building HTML documentation with Sphinx"
	)

endif(SPHINX_FOUND)
