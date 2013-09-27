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


#execute_process(COMMAND ${PROJECT_SOURCE_DIR}/getrevision.sh
#		OUTPUT_VARIABLE GIT_REVISION 
#		OUTPUT_STRIP_TRAILING_WHITESPACE
#		)
#execute_process(COMMAND git describe
#		OUTPUT_VARIABLE GIT_DESCRIBE 
#		OUTPUT_STRIP_TRAILING_WHITESPACE
#		)
#add_definitions(-DGIT_REVISION="${GIT_REVISION}")
#add_definitions(-DGIT_DESCRIBE="${GIT_DESCRIBE}")
#add_definitions(-DGIT_VERSION="${VERSION}")

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
	elseif(UNIX)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
	endif()
endif (NOT MSVC)

# Options
option (WITH_TESTS "Enable testing" OFF)
option (WITH_TESTS_CDASH "Enable cdash testing" OFF)


# Searching for packets
set(DEVELOPER_QT5 1)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Sql REQUIRED)
include_directories( ${Qt5Core_INCLUDE_DIRS} )
include_directories( ${Qt5Network_INCLUDE_DIRS} )
include_directories( ${Qt5Sql_INCLUDE_DIRS} )

SETUP_TESTING()
add_subdirectory(src)
add_subdirectory(tests)
