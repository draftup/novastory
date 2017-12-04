cmake_minimum_required (VERSION 2.8)

set (CMAKE_MODULE_PATH 
	${CMAKE_MODULE_PATH} 
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake"
)
include(cmake/testing.cmake)
include(cmake/qtx.cmake)
include(cmake/doxigenHelper.cmake)

include(cmake/testing.cmake)
include_directories(${CMAKE_SOURCE_DIR})
add_definitions(-DPROJECT_NAME="${PROJECT_NAME}")

set(GIT_REVISION 1)
set(GIT_DESCRIBE "")

#Directories
if (APPLE)
  set (DATA_DIRECTORY "${PROJECT_NAME}.app")
elseif (WIN32)
  set (DATA_DIRECTORY ".")
else (APPLE)
  set (DATA_DIRECTORY "${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}")
endif (APPLE)
add_definitions(-DDATA_DIRECTORY="${DATA_DIRECTORY}")
add_definitions(-DSOURCE_DIRECTORY="${CMAKE_SOURCE_DIR}")
if(WIN32)
  set (BINARY_DIRECTORY .)
else(WIN32)
  set (BINARY_DIRECTORY bin)
endif(WIN32)

# Compilations settings
if (WIN32)
	add_definitions(-DWIN32 -D_WIN32)
endif (WIN32)

configure_file(${CMAKE_SOURCE_DIR}/version.h.in ${CMAKE_SOURCE_DIR}/version.h)

set(FUNCTIONAL_MODULE
	WebServer
)

# Options
option (WITH_TESTS "Enable testing" OFF)
if(WITH_TESTS)
	option (WITH_TESTS_CDASH "Enable cdash testing" OFF)
endif(WITH_TESTS)
option (TOOLKIT_UTIL "Build toolkit for fast windows server compilation" OFF)
option (DISABLE_DATETIME_CHECK "Disable rfc time check" OFF)
if(DISABLE_DATETIME_CHECK)
	add_definitions(-DDISABLE_DATETIME_CHECK)
endif(DISABLE_DATETIME_CHECK)


# Searching for packets
set(DEVELOPER_QT5 1)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5WebSockets REQUIRED)
include_directories( ${Qt5Core_INCLUDE_DIRS} )
include_directories( ${Qt5Network_INCLUDE_DIRS} )
include_directories( ${Qt5Sql_INCLUDE_DIRS} )
include_directories( ${Qt5Xml_INCLUDE_DIRS} )
include_directories( ${Qt5WebSockets_INCLUDE_DIRS} )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
find_package(OpenSSL REQUIRED)
include_directories( ${OPENSSL_INCLUDE_DIR} )

SETUP_TESTING()
add_subdirectory(webserver)
if(WITH_TESTS)
	add_subdirectory(tests)
endif(WITH_TESTS)
if(TOOLKIT_UTIL)
	add_subdirectory(toolkit)
endif(TOOLKIT_UTIL)
