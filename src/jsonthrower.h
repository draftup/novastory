#ifndef JSONERRORTHROWER_H
#define JSONERRORTHROWER_H

#include <QString>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

namespace novastory
{

class JsonThrower
{
public:
	JsonThrower();

	bool isJsonError() const;
	QString jsonErrorDescription() const;
	void jsonReset();
	int jsonErrorType();

	QString jsonString();

	inline void JSON_ERROR(const QString& errorDesc, int error_type = 0)
	{
		qDebug() << errorDesc;
		jsonMainObject.insert("error", QJsonValue(true));
		jsonMainObject.insert("errorDescription", QJsonValue(errorDesc));
		jsonMainObject.insert("errorType", QJsonValue(error_type));
	}

	inline void JSON_INSERT(const QString& key, const QJsonValue& value)
	{
		jsonMainObject.insert(key, value);
	}
private:
	QJsonDocument json;
	QJsonObject jsonMainObject;
};

}

#endif
