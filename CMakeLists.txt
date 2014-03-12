cmake_minimum_required (VERSION 2.8)

# Project settings

set (PROJECT_NAME novastory)
set (PROJECT_VERSION "0.1.0")

# ------------------------------
project(${PROJECT_NAME})
set (CMAKE_MODULE_PATH 
	${CMAKE_MODULE_PATH} 
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake"
)
include(cmake/testing.cmake)
include(cmake/qtx.cmake)
include_directories(${CMAKE_SOURCE_DIR}/src)


execute_process(COMMAND ${CMAKE_SOURCE_DIR}/getrevision.sh
		OUTPUT_VARIABLE GIT_REVISION 
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
execute_process(COMMAND git describe
		OUTPUT_VARIABLE GIT_DESCRIBE 
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
add_definitions(-DGIT_REVISION="${GIT_REVISION}")
add_definitions(-DGIT_DESCRIBE="${GIT_DESCRIBE}")
add_definitions(-DGIT_VERSION="${PROJECT_VERSION}")
message("Git revision detected: ${GIT_REVISION}")
message("Git describe: ${GIT_DESCRIBE}")

#Directories
if (APPLE)
  set (DATA_DIRECTORY "${PROJECT_NAME}.app")
elseif (WIN32)
  set (DATA_DIRECTORY ".")
else (APPLE)
  set (DATA_DIRECTORY "${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}")
endif (APPLE)
add_definitions(-DDATA_DIRECTORY="${DATA_DIRECTORY}")
if(WIN32)
  set (BINARY_DIRECTORY .)
else(WIN32)
  set (BINARY_DIRECTORY bin)
endif(WIN32)

# Compilations settings
if (WIN32)
	add_definitions(-DWIN32 -D_WIN32)
endif (WIN32)
if (NOT MSVC)
	if(APPLE)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++ -g -Wall")
		set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0")
		set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
		set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
	else(APPLE)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
	endif(APPLE)
endif (NOT MSVC)

# Options
option (WITH_TESTS "Enable testing" OFF)
if(WITH_TESTS)
	option (WITH_TESTS_CDASH "Enable cdash testing" OFF)
endif(WITH_TESTS)
option (FEDORA_CROSSCOMPILATION "Enabling fedora crosscompilation features" OFF)


# Searching for packets
set(DEVELOPER_QT5 1)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Sql REQUIRED)
include_directories( ${Qt5Core_INCLUDE_DIRS} )
include_directories( ${Qt5Network_INCLUDE_DIRS} )
include_directories( ${Qt5Sql_INCLUDE_DIRS} )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
find_package(OpenSSL REQUIRED)
include_directories( ${OPENSSL_INCLUDE_DIR} )

SETUP_TESTING()
add_subdirectory(src)
if(WITH_TESTS)
	add_subdirectory(tests)
endif(WITH_TESTS)
