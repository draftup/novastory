cmake_minimum_required(VERSION 2.8)

project(WebServer)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

set(HEADERS_TOMOC_
	webserver.h
	webprocess.h
)

set(HEADERS_
	webdatacontainer.h
	datahandler.h
	templator.h
	webrouter.h
	webrequest.h
)

set(SOURCES_
	webdatacontainer.cpp
	templator.cpp
	webserver.cpp
	webprocess.cpp
	webrouter.cpp
	webrequest.cpp
)

source_group("sources" FILES
	${HEADERS_TOMOC_}
	${HEADERS_}
	${SOURCES_}
)

QT5_WRAP_CPP(MOC_FILES_
	${HEADERS_TOMOC_} 
)

source_group(".generated" FILES
	${MOC_FILES_}
)


add_library(WebServer STATIC
	${HEADERS_TOMOC_}
	${HEADERS_}
	${SOURCES_}
	${MOC_FILES_}
)	

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "webserver")

