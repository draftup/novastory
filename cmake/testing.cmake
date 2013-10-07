macro(SETUP_TESTING)
	if(WITH_TESTS)
		enable_testing()
		add_definitions(-DWITH_TESTS)
		if(WITH_TESTS_CDASH)
			include(CTest)
		endif(WITH_TESTS_CDASH)
		find_package(Qt5Test REQUIRED)
		if(DEVELOPER_QT5)
			include_directories(${Qt5Test_INCLUDE_DIRS})
		else(DEVELOPER_QT5)
			include_directories(${QT_QTTEST_INCLUDE_DIR})
		endif(DEVELOPER_QT5)
	endif(WITH_TESTS)
endmacro(SETUP_TESTING)


# Macro for tests
macro(do_test testname)
	#message(----------------BEGIN----------------)

	# clear variables
	set(RESOURCES_TEST)
	set(SOURCES_TEST)
	set(HDRS_TO_MOC)
	set(SRCS_TO_MOC)
	set(MOC_SRCS)
	set(MOC_HDRS)

	foreach(arg ${ARGN})
		string(REGEX MATCH "qrc_.+\\.cxx" RESORCE ${arg})
		if(NOT "${RESORCE}" STREQUAL "")
			SET(RESOURCES_TEST ${RESOURCES_TEST} ${RESORCE})
		else()
			string(REGEX MATCH ".+\\.(cpp|h|cc|hh|hpp)" SRC ${arg})
			if(NOT "${SRC}" STREQUAL "")
				SET(SOURCES_TEST ${SOURCES_TEST} ${SRC})
			endif()	
		endif()
    endforeach()
	
	# looking for files to moc
	foreach(SRC ${SOURCES_TEST})
		string(REGEX MATCH ".+\\.cpp" SRCMATCH ${SRC})
		if(NOT "${SRCMATCH}" STREQUAL "")
			string(REGEX REPLACE "(.+)(\\.cpp)" "\\1" SRCMATCH ${SRCMATCH})
			
			set(HAS_PAIR 0)
			foreach(SRC2 ${SOURCES_TEST})
				string(REGEX MATCH "${SRCMATCH}\\.h" HEADERMATCH ${SRC2})
				if(NOT "${HEADERMATCH}" STREQUAL "")
					set(HAS_PAIR 1)
				endif()
			endforeach()
		
			if(${HAS_PAIR})
				set(HDRS_TO_MOC ${HDRS_TO_MOC} "${SRCMATCH}.h")
			else()
				set(SRCS_TO_MOC ${SRCS_TO_MOC} "${SRCMATCH}.cpp")
			endif()
			#message(${SRCMATCH})
			
		endif()
	endforeach()
	
	foreach(TOMOC ${SRCS_TO_MOC})
		string(REGEX MATCH "[A-Za-z_\\-]+\\.cpp" TOMOC2 ${TOMOC})
		string(REGEX REPLACE "\\.cpp" ".moc" TEST_MOC_FILE ${TOMOC2})
		set(MOC_${testname}_TEST "${CMAKE_CURRENT_BINARY_DIR}/${TEST_MOC_FILE}")
		QTX_GENERATE_MOC(${TOMOC} ${MOC_${testname}_TEST})
		SET_SOURCE_FILES_PROPERTIES(${TOMOC} PROPERTIES OBJECT_DEPENDS ${MOC_${testname}_TEST})
		set(MOC_SRCS ${MOC_SRCS} ${MOC_${testname}_TEST})
	endforeach()
	
	if(DEFINED HDRS_TO_MOC)
		QTX_WRAP_CPP(MOC_HDRS
			${HDRS_TO_MOC}
		)
	endif()
		
	include_directories(${CMAKE_CURRENT_BINARY_DIR})
		
	add_executable(test_${testname}
		${MOC_HDRS}
		${MOC_SRCS}
		${SOURCES_TEST}
		${RESOURCES_TEST}
	)

	if(DEVELOPER_QT5)
		set(QTX_TEST_LIBRARY ${Qt5Test_LIBRARIES})
	else(DEVELOPER_QT5)
		set(QTX_TEST_LIBRARY ${QT_QTTEST_LIBRARY})
	endif(DEVELOPER_QT5)

	target_link_libraries(test_${testname}
		${FUNCTIONAL_MODULE}
		${QTX_TEST_LIBRARY}
	)

	set_property(TARGET test_${testname} PROPERTY FOLDER "Tests")

	ADD_TEST(test_${testname} test_${testname})
endmacro(do_test testname)

macro(append_test_resource testname)
	set(test_resource ${ARGV1})
	get_target_property(TEST_BINARY_FILE test_${testname} LOCATION)
	get_filename_component(TEST_BINARY_PATH ${TEST_BINARY_FILE} PATH)
	add_custom_command(TARGET test_${testname} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy \"${test_resource}\" ${TEST_BINARY_PATH}/)
endmacro(append_test_resource testname)

