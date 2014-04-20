#include "sqlquery.h"
#include <QDebug>
#include <QSqlError>
#include <QThread>
#include "sqldatabase.h"

namespace novastory
{

bool SqlQuery::exec(const QString& query)
{
	bool result = QSqlQuery::exec(query);
	if (result)
	{
		qDebug() << "SQL EXEC [OK]: " << query;
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
	bool result = QSqlQuery::exec();
	QList<QVariant> list = boundValues().values();
	if (result)
	{
		qDebug() << "SQL EXEC [OK]: " << lastQuery();
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

SqlQuery::SqlQuery(const QString& query /*= QString()*/) : QSqlQuery(query, SqlDatabase::open(QThread::currentThreadId()))
{
	if (!query.isEmpty())
	{
		QSqlError error = lastError();
		if (!error.isValid())
		{
			qDebug() << "SQL EXEC [OK]: " << lastQuery();
		}
		else
		{
			qCritical() << "SQL EXEC [FAIL]: " << lastQuery();
			qCritical() << "SQL Error: " << lastError();
		}
	}
}

}
