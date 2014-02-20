#include "sqldatabase.h"
#include "utils/globals.h"

namespace novastory
{

SqlDatabase::SqlDatabase(bool openOnStart) : QSqlDatabase(addDatabase("QMYSQL"))
{
	setHostName("localhost");
	setDatabaseName("novastory");
	setUserName("root");
	setPassword("degitisi");
	if(openOnStart)
		VERIFY(open());
}

SqlDatabase::~SqlDatabase()
{

}

}
