#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include <QSqlDatabase>
#include <QHash>
#include <QDebug>
#include <QMutex>

namespace novastory
{

class SqlDatabase : public QSqlDatabase
{
public:
	SqlDatabase(const SqlDatabase& olddb);
	SqlDatabase(const QSqlDatabase& olddb);
	virtual ~SqlDatabase();
	static SqlDatabase open(Qt::HANDLE threadId);
	static void close(Qt::HANDLE threadId);
};

}

#endif
