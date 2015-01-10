cmake_minimum_required(VERSION 2.8)
# Eliminate a warning when building in Windows that relates
# to static linking of Qt executables to qtmain.lib.
# This policy was introduced in CMake version 2.8.11.
# CMake version 2.8.11.2 warns when the policy is not set
# and uses OLD behavior.
if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()

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
	recaptcha.h
	
	logic/user.h
	logic/captcha.h
	logic/texteditor.h
	logic/textrevision.h
	logic/avatar.h
	logic/webdata.h
	logic/userpic.h
	logic/bookinfo.h
)

set(HEADERS
	database/novastory_db.h
	
	logic/textrevisioncontainer.h
	logic/loggedactions.h

	jsonthrower.h
	config.h
	avatarshandler.h
	userspicshandler.h
	registrationhandler.h
	indexhandler.h
	profilehandler.h
	profile.h
	htmltemplate.h
	apihandler.h
	novastoryerrorhandler.h
	novastoryserver.h
)

set(SOURCES
	logic/user.cpp
	logic/captcha.cpp
	logic/texteditor.cpp
	logic/textrevision.cpp
	logic/textrevisioncontainer.cpp
	logic/avatar.cpp
	logic/webdata.cpp
	logic/userpic.cpp
	logic/bookinfo.cpp
	logic/loggedactions.cpp
	
	recaptcha.cpp
	avatarshandler.cpp
	userspicshandler.cpp
	registrationhandler.cpp
	indexhandler.cpp
	profilehandler.cpp
	profile.cpp
	apihandler.cpp
	jsonthrower.cpp
	novastoryserver.cpp
	novastoryerrorhandler.cpp
)

add_subdirectory(webserver)

QT5_WRAP_CPP(MOC_FILES ${HEADERS_TOMOC})

if(WIN32 OR APPLE)
	#QT5_ADD_RESOURCES(QRC_RESOURCES
	#	${CMAKE_CURRENT_SOURCE_DIR}/resources/qtconf.qrc
	#)
endif()

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
	WebServer
	${OPENSSL_LIBRARIES}
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
