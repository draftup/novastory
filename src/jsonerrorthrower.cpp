#include "jsonerrorthrower.h"
#include <QJsonDocument>
#include <QJsonObject>

novastory::JsonErrorThrower::JsonErrorThrower() : isError(false), errorType(0)
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
	errorType = 0;
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

void novastory::JsonErrorThrower::errorTypeReset()
{
	errorType = 0;
}

int novastory::JsonErrorThrower::jsonErrorType()
{
	return errorType;
}
