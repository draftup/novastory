#include "sqlquery.h"
#include <QDebug>
#include <QSqlError>

namespace novastory
{

	bool SqlQuery::exec( const QString& query )
	{
		bool result = QSqlQuery::exec(query);
		if(result)
			qDebug() << "SQL EXEC [OK]: " << query;
		else
		{
			qWarning() << "SQL EXEC [FAIL]: " << query;
			qWarning() << "SQL Error: " << lastError();
		}

		return result;
	}

	bool SqlQuery::exec()
	{
		bool result = QSqlQuery::exec();
		if(result)
			qDebug() << "SQL EXEC [OK]: " << lastQuery();
			QList<QVariant> list = boundValues().values();
			if(list.size() > 0)
			{
				qDebug() << "SQL Binded values:";
				for(int i = 0; i < list.size(); ++i)
					qDebug() << i << ": " << list.at(i);
			}
		else
		{
			qWarning() << "SQL EXEC [FAIL]: " << lastQuery();
			qWarning() << "SQL Error: " << lastError();
			QList<QVariant> list = boundValues().values();
			if(list.size() > 0)
			{
				qWarning() << "SQL Binded values:";
				for(int i = 0; i < list.size(); ++i)
					qWarning() << i << ": " << list.at(i);
			}
			
		}

		return result;
	}

	SqlQuery::SqlQuery( const QString & query /*= QString()*/, QSqlDatabase db /*= QSqlDatabase()*/ ) : QSqlQuery(query, db)
	{
		if(!query.isEmpty())
		{
			QSqlError error = lastError();
			if(!error.isValid())
				qDebug() << "SQL EXEC [OK]: " << lastQuery();
			else
			{
				qWarning() << "SQL EXEC [FAIL]: " << lastQuery();
				qWarning() << "SQL Error: " << lastError();
			}
		}
	}

}
