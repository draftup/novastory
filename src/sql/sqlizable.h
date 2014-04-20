#ifndef SQLIZABLE_H
#define SQLIZABLE_H

#include <QObject>
#include "sql/sqlquery.h"
#include <QJsonObject>

namespace novastory
{

class Sqlizable : public QObject
{
	Q_OBJECT
public:
	Sqlizable();
	~Sqlizable() {};

	bool insertSQL();
	bool updateSQL(const QList<QString>& basis, const QList<QString>& ignoreVariables = QList<QString>());
	bool updateSQL(const QString& basis, const QList<QString>& ignoreVariables = QList<QString>());
	bool removeSQL(const QList<QString>& basis);
	bool removeSQL(const QString& basis);
	bool syncSQL(const QList<QString>& basis);
	bool syncSQL(const QString& basis);

	Sqlizable& operator =(const Sqlizable&);

	QJsonObject jsonObject() const;
protected:
	bool syncProcess(SqlQuery& query);
};

}

#endif
