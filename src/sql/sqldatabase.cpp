#include "sqldatabase.h"
#include "utils/globals.h"
#include "config.h"

namespace novastory
{

SqlDatabase::SqlDatabase(const QSqlDatabase& olddb) : QSqlDatabase(olddb)
{
}

SqlDatabase::~SqlDatabase()
{

}

SqlDatabase& SqlDatabase::open( Qt::HANDLE threadId )
{
	QString id = QString::number((unsigned long long) threadId);
	if(contains(id))
	{
		return SqlDatabase(database(id));
	}

	QSqlDatabase newdb = addDatabase("QMYSQL", id);
	newdb.setConnectOptions("MYSQL_OPT_RECONNECT=1");
	newdb.setHostName(MYSQL_HOST);
	newdb.setDatabaseName(MYSQL_DATABASE);
	newdb.setUserName(MYSQL_USER);
	newdb.setPassword(MYSQL_PASSWORD);
	VERIFY(newdb.open());
	
	qDebug() << "New database connection opened (current connections:" << connectionNames().size() << ")";

	return SqlDatabase(newdb);
}

void SqlDatabase::close( Qt::HANDLE threadId )
{
	QString id = QString::number((unsigned long long) threadId);
	if(contains(id))
	{
		removeDatabase(id);
		qDebug() << "Database connection closed (current connections:" << connectionNames().size() << ")";
	}
}

}
