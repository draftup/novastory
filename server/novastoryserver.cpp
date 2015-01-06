#include "novastoryserver.h"
#include "rawfilehandler.h"
#include "registrationhandler.h"
#include "apihandler.h"
#include "avatarshandler.h"
#include "userspicshandler.h"
#include "profilehandler.h"
#include "indexhandler.h"

namespace novastory
{


NovastoryServer::NovastoryServer(QObject* parent, quint16 initializationPort) : WebServer::WebServer(parent, initializationPort)
{
	appendHandler(new RawFileHandler(bindedSocket));
	appendHandler(new IndexHandler(bindedSocket));
	appendHandler(new ApiHandler(bindedSocket));
	appendHandler(new RegistrationHandler(bindedSocket));
	appendHandler(new AvatarsHandler(bindedSocket));
	appendHandler(new UsersPicsHandler(bindedSocket));
	appendHandler(new ProfileHandler(bindedSocket));
}

}
