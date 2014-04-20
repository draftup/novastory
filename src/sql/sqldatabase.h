#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include <QSqlDatabase>
#include <QHash>
#include <QDebug>

namespace novastory
{

class SqlDatabase : public QSqlDatabase
{
public:
	SqlDatabase(bool openOnStart = false, const QString& connectionName = QLatin1String(defaultConnection));
	virtual ~SqlDatabase();
};

struct Databases
{
	SqlDatabase& open(Qt::HANDLE threadId)
	{
		QMutexLocker locker(&databasesLocker);
		if (databases.contains(threadId))
		{
			return databases[threadId];
		}

		qDebug() << "New database connection opened (current connections:" << databases.size() + 1 << ")";

		return databases.insert(threadId, SqlDatabase(true, QString::number((unsigned long long) threadId))).value();
	}

	void close(Qt::HANDLE threadId)
	{
		QMutexLocker locker(&databasesLocker);
		if (databases.contains(threadId))
		{
			qDebug() << "Database connection closed (current connections:" << databases.size() - 1 << ")";
		}
		databases.remove(threadId);
	}

	static Databases& Instance()
	{
		static Databases theSingleInstance;
		return theSingleInstance;
	}
private:
	QHash<Qt::HANDLE, SqlDatabase> databases;
	QMutex databasesLocker;
};

}

#endif
