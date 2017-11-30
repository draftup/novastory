#ifndef SQLQUERY_H
#define SQLQUERY_H

#include <QSqlQuery>

namespace novastory
{

class SqlQuery : public QSqlQuery
{
public:
	SqlQuery(QSqlResult* result) : QSqlQuery(result) {};
	SqlQuery(const QString& query = QString());
	SqlQuery(QSqlDatabase db) : QSqlQuery(db) {};
	SqlQuery(const QSqlQuery& other) : QSqlQuery(other) {};
	virtual ~SqlQuery() {};

	bool exec(const QString& query);
	bool exec();
};

}

#endif
