#ifndef SQLIZABLE_H
#define SQLIZABLE_H

#include <QObject>
#include "sqlquery.h"
#include <QJsonObject>
#include <QVariant>
#include <QHash>

namespace novastory
{

class Sqlizable : public QObject
{
	Q_OBJECT
public:
	Sqlizable();
	Sqlizable(const novastory::Sqlizable&);
	~Sqlizable() {};

	bool insertSQL(bool can_dublicate = false, bool not_ignore_id = false);
	bool updateSQL(const QList<QString>& basis, const QList<QString>& ignoreVariables = QList<QString>());
	bool updateSQL(const QString& basis, const QList<QString>& ignoreVariables = QList<QString>());
	bool removeSQL(const QList<QString>& basis);
	bool removeSQL(const QString& basis);
	bool syncSQL(const QList<QString>& basis, const QString& join_query = QString(), const QString& select_query = QString());
	bool syncSQL(const QString& basis, const QString& join_query = QString(), const QString& select_query = QString());
	bool syncSQL(const QHash<QString, QVariant>& basis, const QString& join_query = QString(), const QString& select_query = QString());

	void substitute(QString& data, QString prefix = QString()) const;

	Sqlizable& operator =(const Sqlizable&);

	QByteArray toJson() const;
	virtual QJsonObject jsonObject() const;
	virtual void fromJson(const QByteArray& json);
	virtual void fromJson(const QJsonObject& json);

	static bool isObjectType(int type)
	{
		const int objectTypeIds[] =
		{
			qMetaTypeId<QObjectList>(),
			qMetaTypeId<QObject*>()
		};

		return std::find(std::begin(objectTypeIds), std::end(objectTypeIds), type) != std::end(objectTypeIds);
	}

	static QString escapeString(const QString& string);
	static QString inList(const QList<int>& list);
	static QString inList(const QList<QString>& list);
	static QString inField(const QString& name, const QList<int>& list);
protected:
	virtual bool syncProcess(SqlQuery& query);
	virtual int syncRecord(SqlQuery& query, bool found_mode = false, int from = -1, int to = -1);
	virtual int syncRecord(QSqlRecord& query, bool found_mode = false, int from = -1, int to = -1);
private:
	QVariant CORRECT_JSON_VARIANT(const QVariant& data) const;
};

}

QDataStream& operator<< (QDataStream& stream, const novastory::Sqlizable& obj);
QDataStream& operator>> (QDataStream& stream, novastory::Sqlizable& obj);
QDataStream& operator<< (QDataStream& stream, const novastory::Sqlizable* obj);
QDataStream& operator>> (QDataStream& stream, novastory::Sqlizable* obj);


#endif
