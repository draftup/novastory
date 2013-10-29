#include "sqldatabase.h"


novastory::SqlDatabase::SqlDatabase() : QSqlDatabase(addDatabase("QMYSQL"))
{
	setHostName("acidalia");
	setDatabaseName("customdb");
	setUserName("mojito");
	setPassword("J0a1m8");
}

novastory::SqlDatabase::~SqlDatabase()
{

}
