#ifndef SQLIZABLE_H
#define SQLIZABLE_H

#include <QObject>
#include "sql/sqlquery.h"
#include <QJsonObject>
#include <QVariant>

namespace novastory
{

class Sqlizable : public QObject
{
	Q_OBJECT
public:
	Sqlizable();
	Sqlizable(const novastory::Sqlizable&);
	~Sqlizable() {};

	bool insertSQL();
	bool updateSQL(const QList<QString>& basis, const QList<QString>& ignoreVariables = QList<QString>());
	bool updateSQL(const QString& basis, const QList<QString>& ignoreVariables = QList<QString>());
	bool removeSQL(const QList<QString>& basis);
	bool removeSQL(const QString& basis);
	bool syncSQL(const QList<QString>& basis);
	bool syncSQL(const QString& basis);

	void substitute(QString& data, QString prefix = QString()) const;

	Sqlizable& operator =(const Sqlizable&);

	QJsonObject jsonObject() const;

	static bool isObjectType(int type)
	{
		const int objectTypeIds[] =
		{
			qMetaTypeId<QObjectList>(),
			qMetaTypeId<QObject*>(),
			QVariant::ByteArray,
		};

		return std::find(std::begin(objectTypeIds), std::end(objectTypeIds), type) != std::end(objectTypeIds);
	}
protected:
	bool syncProcess(SqlQuery& query);
};

}

#endif
