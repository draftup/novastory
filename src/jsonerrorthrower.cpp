#include "jsonerrorthrower.h"
#include <QJsonDocument>
#include <QJsonObject>

novastory::JsonErrorThrower::JsonErrorThrower() : isError(false)
{

}

bool novastory::JsonErrorThrower::isJsonError() const
{
	return isError;
}

QString novastory::JsonErrorThrower::jsonErrorDescription() const
{
	return errorDescription;
}

void novastory::JsonErrorThrower::jsonReset()
{
	isError = false;
	errorDescription = QString();
}

QString novastory::JsonErrorThrower::jsonErrorString() const
{
	QJsonDocument json;
	QJsonObject errorObject;
	
	errorObject.insert("error", QJsonValue(isError));
	errorObject.insert("errorDescription", QJsonValue(errorDescription));
	
	json.setObject(errorObject);
	return json.toJson();
}
