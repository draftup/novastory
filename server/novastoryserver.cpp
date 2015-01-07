#include "novastoryserver.h"
#include "webserver/routers/rawfilehandler.h"
#include "registrationhandler.h"
#include "apihandler.h"
#include "avatarshandler.h"
#include "userspicshandler.h"
#include "profilehandler.h"
#include "indexhandler.h"

namespace novastory
{


NovastoryServer::NovastoryServer(QObject* parent, quint16 initializationPort) : WebServer(parent, initializationPort)
{
	appendHandler(new RawFileHandler());
	appendHandler(new IndexHandler());
	appendHandler(new ApiHandler());
	appendHandler(new RegistrationHandler());
	appendHandler(new AvatarsHandler());
	appendHandler(new UsersPicsHandler());
	appendHandler(new ProfileHandler());
}

NovastoryServer::~NovastoryServer()
{

}

}
