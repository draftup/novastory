cmake_minimum_required(VERSION 2.8)
# Eliminate a warning when building in Windows that relates
# to static linking of Qt executables to qtmain.lib.
# This policy was introduced in CMake version 2.8.11.
# CMake version 2.8.11.2 warns when the policy is not set
# and uses OLD behavior.
if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

set(HEADERS_TOMOC_
	webserver.h
	webprocess.h
	cron.h
	smtpsender.h
	
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
	cron.cpp
	smtpsender.cpp
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
	${Qt5Core_LIBRARIES}
	${Qt5Network_LIBRARIES}
	${Qt5Sql_LIBRARIES}
)


set_property(TARGET WebServer PROPERTY FOLDER "webserver")

