#include "sqldatabase.h"

namespace novastory
{

SqlDatabase::SqlDatabase() : QSqlDatabase(addDatabase("QMYSQL"))
{
	setHostName("localhost");
	setDatabaseName("novastory");
	setUserName("root");
	setPassword("degitisi");
}

SqlDatabase::~SqlDatabase()
{

}

}
