#include "sqldatabase.h"
#include "utils/globals.h"
#include "config.h"

namespace novastory
{

SqlDatabase::SqlDatabase(bool openOnStart, const QString & connectionName) : QSqlDatabase(addDatabase("QMYSQL", connectionName))
{
	setConnectOptions("MYSQL_OPT_RECONNECT=1");
	setHostName(MYSQL_HOST);
	setDatabaseName(MYSQL_DATABASE);
	setUserName(MYSQL_USER);
	setPassword(MYSQL_PASSWORD);
	if (openOnStart)
	{
		VERIFY(open());
	}
}

SqlDatabase::~SqlDatabase()
{

}

}
