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
	webserver.h
	sql/sqlizable.h

	logic/users.h
)

set(HEADERS
	sql/sqlquery.h
	sql/sqldatabase.h
	
	datahandler.h
	rawfilehandler.h
	logger.h
	apihandler.h
	webrouter.h
	webrequest.h
)

set(SOURCES
	sql/sqlquery.cpp
	sql/sqldatabase.cpp
	sql/sqlizable.cpp
	
	logic/users.cpp
	
	rawfilehandler.cpp
	apihandler.cpp
	logger.cpp
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

install (TARGETS ${SERVER_MODULE} DESTINATION ${BINARY_DIRECTORY})
install (DIRECTORY ${CMAKE_SOURCE_DIR}/public/ DESTINATION "${DATA_DIRECTORY}/public")

if(FEDORA_CROSSCOMPILATION)
	install(FILES
				/usr/x86_64-w64-mingw32/sys-root/mingw/bin/libgcc_s_seh-1.dll
				/usr/x86_64-w64-mingw32/sys-root/mingw/bin/libstdc++-6.dll
				/usr/x86_64-w64-mingw32/sys-root/mingw/bin/libpcre16-0.dll
				/usr/x86_64-w64-mingw32/sys-root/mingw/bin/Qt5Core.dll
				/usr/x86_64-w64-mingw32/sys-root/mingw/bin/Qt5Network.dll
				/usr/x86_64-w64-mingw32/sys-root/mingw/bin/zlib1.dll
			DESTINATION .
		)
endif(FEDORA_CROSSCOMPILATION)
