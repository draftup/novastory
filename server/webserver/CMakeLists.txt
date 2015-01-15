cmake_minimum_required(VERSION 2.8)

project(WebServer)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

set(HEADERS_TOMOC_
	webserver.h
	webprocess.h
	
	sql/sqlizable.h
)

set(HEADERS_
	webdatacontainer.h
	datahandler.h
	templator.h
	webrouter.h
	webrequest.h
	bytecache.hpp
	lrucache.hpp
	globals.h
	webserver_config.h
	jsonthrower.h
	
	logger.h
	
	routers/rawfilehandler.h
	routers/errorhandler.h
	
	sql/sqlquery.h
	sql/sqldatabase.h
	sql/dbpatcher.h
	sql/nestedset.h
)

set(SOURCES_
	globals.cpp
	webdatacontainer.cpp
	templator.cpp
	webserver.cpp
	webprocess.cpp
	webrouter.cpp
	webrequest.cpp
	jsonthrower.cpp
	
	logger.cpp
	routers/rawfilehandler.cpp
	routers/errorhandler.cpp
	
	sql/sqlquery.cpp
	sql/sqldatabase.cpp
	sql/sqlizable.cpp
	sql/dbpatcher.cpp
	sql/nestedset.cpp
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

add_subdirectory(3dparty/smtp)
include_directories(3dparty/smtp)

add_library(WebServer STATIC
	${HEADERS_TOMOC_}
	${HEADERS_}
	${SOURCES_}
	${MOC_FILES_}
)

target_link_libraries(WebServer
	smtpemail
)


set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "webserver")

