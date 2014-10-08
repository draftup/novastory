cmake_minimum_required(VERSION 2.8)
# Eliminate a warning when building in Windows that relates
# to static linking of Qt executables to qtmain.lib.
# This policy was introduced in CMake version 2.8.11.
# CMake version 2.8.11.2 warns when the policy is not set
# and uses OLD behavior.
if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)


set(HEADERS_TOMOC

)

set(HEADERS

)

set(SOURCES
	main.cpp
)


#QT5_WRAP_CPP(MOC_FILES ${HEADERS_TOMOC})

source_group("core\\Sources" FILES 
				${HEADERS} 
				${HEADERS_TOMOC}
				${SOURCES})		
				
ADD_EXECUTABLE(toolkit
	${DESKTOP_TARGET}
	
	# Sources
	${HEADERS_TOMOC}
	${HEADERS}
	${SOURCES}
)
target_link_libraries(toolkit
	${Qt5Core_LIBRARIES}
	${OPENSSL_LIBRARIES}
)


install (TARGETS toolkit DESTINATION ${CMAKE_SOURCE_DIR}/utils)
