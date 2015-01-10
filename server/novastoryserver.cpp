#include "novastoryserver.h"
#include "webserver/routers/rawfilehandler.h"
#include "novastoryerrorhandler.h"
#include "registrationhandler.h"
#include "apihandler.h"
#include "avatarshandler.h"
#include "userspicshandler.h"
#include "profilehandler.h"
#include "indexhandler.h"

#include "database/novastory_db.h"

namespace novastory
{


NovastoryServer::NovastoryServer(QObject* parent, quint16 initializationPort, const QString& pid_file, const QString& db_file) : WebServer(parent, initializationPort, pid_file, db_file)
{
	// Update db to last version
	DBPatcher patcher;
	patcher.setDatabaseStructure(DB_TABLE_STRUCT());
	patcher.patch();

	appendHandler(new RawFileHandler());
	appendHandler(new NovastoryErrorHandler());
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
