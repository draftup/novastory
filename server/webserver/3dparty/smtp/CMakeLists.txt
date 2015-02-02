cmake_minimum_required (VERSION 2.8)
if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()
IF(POLICY CMP0048)
	cmake_policy(SET CMP0048 NEW)
ENDIF(POLICY CMP0048)
project (SMTPEmail VERSION 1.1)

set(SMTP_MOC_HDRS
	emailaddress.h
	mimeattachment.h
	mimecontentformatter.h
	mimefile.h
	mimehtml.h
	mimemultipart.h
	mimepart.h
	quotedprintable.h
	smtpclient.h
)

set (SMTP_SRCS 
	emailaddress.cpp
	mimeattachment.cpp
	mimecontentformatter.cpp
	mimefile.cpp
	mimehtml.cpp
	mimeinlinefile.cpp
	mimemessage.cpp
	mimemultipart.cpp
	mimepart.cpp
	mimetext.cpp
	quotedprintable.cpp
	smtpclient.cpp
)
set (SMTP_HEADERS 
	mimeinlinefile.h
	mimemessage.h
	mimetext.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
QT5_WRAP_CPP(SMTP_MOC_SRCS ${SMTP_MOC_HDRS})

add_library (smtpemail STATIC ${SMTP_SRCS} ${SMTP_HEADERS} ${SMTP_MOC_SRCS} ${SMTP_MOC_HDRS})
target_link_libraries(smtpemail ${OPENSSL_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Network_LIBRARIES})
