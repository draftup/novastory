cmake_minimum_required(VERSION 2.8)

project(Video)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)


set(HEADERS_LOCAL_TOMOC
	sql/sqlquery.h	

	webserver.h
)

set(HEADERS_LOCAL
	webrouter.h
	webrequest.h
)

set(SOURCES_LOCAL
	sql/sqlquery.cpp

	webserver.cpp
	webrouter.cpp
	webrequest.cpp
	main.cpp
)

QT5_WRAP_CPP(MOC_LOCAL ${HEADERS_LOCAL_TOMOC})

source_group("core\\Sources" FILES 
				${HEADERS_LOCAL} 
				${HEADERS_LOCAL_TOMOC}
				${SOURCES_LOCAL})

ADD_EXECUTABLE(novaserver
	${SOURCES_LOCAL}
	${MOC_LOCAL}
	${HEADERS_LOCAL}
	${HEADERS_LOCAL_TOMOC}
)
target_link_libraries (novaserver
	#${Qt5Core_QTMAIN_LIBRARIES}
	${Qt5Core_LIBRARIES}
	${Qt5Network_LIBRARIES}
	${Qt5Sql_LIBRARIES}
)
