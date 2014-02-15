#include "sqlizable.h"
#include "sqlquery.h"
#include "utils/globals.h"
#include <QSqlRecord>
#include <QMetaObject>
#include <QMetaProperty>
#include <QVariant>
#include <QList>
#include <QDebug>


namespace novastory
{
Sqlizable::Sqlizable() : QObject()
{
	setObjectName("SQLizable_BASE");
}

bool isObjectType(int type)
{
	const int objectTypeIds[] =
	{
		qMetaTypeId<QObjectList>(),
		qMetaTypeId<QObject*>(),
		QVariant::ByteArray,
	};

	return std::find(std::begin(objectTypeIds), std::end(objectTypeIds), type) != std::end(objectTypeIds);
}

/*
bool skipCleanVariant(const QVariant& variant)
{
	QVariant::Type type = variant.type();

}
*/

bool Sqlizable::insertSQL()
{
	const QMetaObject* mObject = metaObject();
	const int propCount = mObject->propertyCount();
	if (propCount > 0)
	{
		QList<QVariant> values;

		QString objName = objectName();
		if (objName.isEmpty())
		{
			return false; // No object name, we doesn't know name of table
		}

		QVariant auto_increment = property("auto_increment");
		bool isAutoIncrement = auto_increment.isValid();

		QString sql = "INSERT INTO `" + objName + "`(";

		for (int i = 0; i < propCount; ++i)
		{
			QMetaProperty prop = mObject->property(i);

			if (isObjectType(prop.userType()))
			{
				continue;
			}

			const QString& propName = prop.name();
			QVariant propValue = prop.read(this);

			// Skip objectName
			if (propName == "objectName")
			{
				continue;
			}

			// Skip auto_increment value
			if (isAutoIncrement)
			{
				if (propName == auto_increment.toString())
				{
					continue;
				}
			}

			values.append(propValue);

			sql += (i == propCount - 1) ? "`" + propName + "`" : "`" + propName + "`, ";
		}

		int valuesCout = values.count();

		sql += ") VALUES(";
		for (int i = 0; i < valuesCout; ++i)
		{
			sql += (i == valuesCout - 1) ? "?" : "?, ";
		}
		sql += ")";

		SqlQuery query;
		query.prepare(sql);

		int paramNumber = 0;
		for (int i = 0; i < valuesCout; ++i)
		{
			query.bindValue(i, values.at(i));
		}

		bool result = query.exec();

		// Auto increment update proprerty
		if (isAutoIncrement)
		{
			setProperty(auto_increment.toString().toUtf8().data(), query.lastInsertId());
		}


		return result;
	}

	return true;
}

bool Sqlizable::syncSQL(QList<QString> basis)
{
	QString objName = objectName();
	if (objName.isEmpty())
	{
		return false; // No object name, we doesn't know name of table
	}

	SqlQuery query;

	QString sql = QString("SELECT * FROM `") + objName + "` WHERE ";

	// Founding keys
	QList<QVariant> internalValues;
	/*
	if(property("primary_key").isValid() && property(property("primary_key").toString().toUtf8()).isValid())
	{
		sql += "`" + property("primary_key").toString() + "` = ?";
		internalValues.append(property(property("primary_key").toString().toUtf8()).toString());
	}
	else
	{
		const QMetaObject* mObject = metaObject();
		const int propCount = mObject->propertyCount();
		for (int i = 0; i < propCount; ++i)
		{
			QMetaProperty prop = mObject->property(i);
			const QString& propName = prop.name();
			QVariant propValue = prop.read(this);

			if(!propValue.isValid())
			{
				continue;
			}

			// Skip objectName
			if (propName == "objectName")
			{
				continue;
			}

			sql += (internalValues.size() == 0) ? propName + " = ?" : " AND " + propName + " = ?";
			internalValues.append(propValue);
		}
	}
	*/
	for(QString& questVar : basis)
	{
		QVariant propValue = property(questVar.toUtf8());
		sql += (internalValues.size() == 0) ? questVar + " = ?" : " AND " + questVar + " = ?";
		internalValues.append(propValue);
	}

	VERIFY(query.prepare(sql));

		for(int i = 0; i < internalValues.size(); ++i)
		{
			query.bindValue(i, internalValues[i]);
		}

	if(!query.exec())
	{
		qWarning() << "No sync record from" << objName << "founded";
		return false;
	}

	if(query.size() != 1)
	{
		qWarning() << "Not only one record in database. Can not sync.";
		return false;
	}

	VERIFY(query.next());
	QSqlRecord record = query.record();
	for(int i = 0; i < record.count(); ++i)
	{
		QString propertyName = record.fieldName(i);
		QVariant propertyValue = record.value(i);
		setProperty(propertyName.toLatin1(), propertyValue);
		qDebug() << "SYNCSQL" << objName << ": " << propertyName << "=" << propertyValue.toString(); 
	}

	return true;
}

}


