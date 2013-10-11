cmake_minimum_required(VERSION 2.8)

set(FUNCTIONAL_MODULE
	novastory CACHE STRING "Module with functional of the program"
)
set(SERVER_MODULE
	novastory_server CACHE STRING "Module with functional of the program"
)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)


set(HEADERS_TOMOC
	sql/sqlquery.h
	webserver.h
)

set(HEADERS
	datahandler.h
	rawfilehandler.h
	webrouter.h
	webrequest.h
)

set(SOURCES
	sql/sqlquery.cpp
	rawfilehandler.cpp
	webserver.cpp
	webrouter.cpp
	webrequest.cpp
)

QT5_WRAP_CPP(MOC_FILES ${HEADERS_TOMOC})

source_group("core\\Sources" FILES 
				${HEADERS} 
				${HEADERS_TOMOC}
				${SOURCES})

add_library(${FUNCTIONAL_MODULE} STATIC
	# Sources
	${HEADERS_TOMOC}
	${HEADERS}
	${SOURCES}

	# Genered resources
	${UI_FILES} 
	${MOC_FILES} 
	${QRC_RESOURCES}
	${QM_FILES}

	# Some source resources
	${TS_FILES}
	${CSS_FILES}
)

target_link_libraries(${FUNCTIONAL_MODULE}
	${Qt5Core_LIBRARIES}
	${Qt5Network_LIBRARIES}
	${Qt5Sql_LIBRARIES}
)
				
				
ADD_EXECUTABLE(${SERVER_MODULE}
	${DESKTOP_TARGET}
	main.cpp
	
	# Genered resources
	${QRC_RESOURCES}
	${RESOURCE_OS}
	${RESOURCE_OS_ALL}
)
target_link_libraries (${SERVER_MODULE}
	${FUNCTIONAL_MODULE}
)
