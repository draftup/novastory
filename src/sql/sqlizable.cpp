#include "sqlizable.h"
#include "sqlquery.h"
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
			return true; // No object name, we doesn't know name of table
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

}


