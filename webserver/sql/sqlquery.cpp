#include "sqlquery.h"
#include <QDebug>
#include <QSqlError>
#include <QThread>
#include "sqldatabase.h"
#include <QElapsedTimer>

namespace novastory
{

bool SqlQuery::exec(const QString& query)
{
	QElapsedTimer timer;
	timer.start();
	bool result = QSqlQuery::exec(query);
	if (result)
	{
		qDebug() << "SQL EXEC [OK]: " << query << "[time = " + QString::number((double)timer.nsecsElapsed() / 1000) + "qs]";
	}
	else
	{
		qCritical() << "SQL EXEC [FAIL]: " << query;
		qCritical() << "SQL Error: " << lastError();
	}

	return result;
}

bool SqlQuery::exec()
{
	QElapsedTimer timer;
	timer.start();
	bool result = QSqlQuery::exec();
	QList<QVariant> list = boundValues().values();
	if (result)
	{
		qDebug() << "SQL EXEC [OK]: " << lastQuery() << "[time = " + QString::number((double)timer.nsecsElapsed() / 1000) + "qs]";
		if (list.size() > 0)
		{
			qDebug() << "SQL Binded values:";
		}
		for (int i = 0; i < list.size(); ++i)
		{
			qDebug() << i << ": " << list.at(i);
		}
	}
	else
	{
		qCritical() << "SQL EXEC [FAIL]: " << lastQuery();
		qCritical() << "SQL Error: " << lastError();
		if (list.size() > 0)
		{
			qCritical() << "SQL Binded values:";
		}
		for (int i = 0; i < list.size(); ++i)
		{
			qCritical() << i << ": " << list.at(i);
		}
	}

	return result;
}

SqlQuery::SqlQuery(const QString& query /*= QString()*/)
{
	QElapsedTimer timer;
	timer.start();
	*this = QSqlQuery(query, SqlDatabase::open(QThread::currentThreadId()));
	if (!query.isEmpty())
	{
		QSqlError error = lastError();
		if (!error.isValid())
		{
			qDebug() << "SQL EXEC [OK]: " << lastQuery() << "[time = " + QString::number((double)timer.nsecsElapsed() / 1000) + "qs]";
		}
		else
		{
			qCritical() << "SQL EXEC [FAIL]: " << lastQuery();
			qCritical() << "SQL Error: " << lastError();
		}
	}
}

}
