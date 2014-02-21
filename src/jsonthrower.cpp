#include "jsonthrower.h"
#include <QJsonObject>

novastory::JsonThrower::JsonThrower()
{
	jsonReset();
}

bool novastory::JsonThrower::isJsonError() const
{
	return jsonMainObject.value("error").toBool();
}

QString novastory::JsonThrower::jsonErrorDescription() const
{
	return jsonMainObject.value("errorDescription").toString();
}

void novastory::JsonThrower::jsonReset()
{
	while (jsonMainObject.size())
	{
		jsonMainObject.erase(jsonMainObject.begin());
	}
	jsonMainObject.insert("error", QJsonValue(false));
	jsonMainObject.insert("errorDescription", QJsonValue());
	jsonMainObject.insert("errorType", QJsonValue());
}

QString novastory::JsonThrower::jsonString()
{
	json.setObject(jsonMainObject);
	return json.toJson();
}

int novastory::JsonThrower::jsonErrorType()
{
	return jsonMainObject.value("errorType").toInt();
}
