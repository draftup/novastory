#ifndef JSONERRORTHROWER_H
#define JSONERRORTHROWER_H

#include <QString>
#include <QDebug>

namespace novastory
{

class JsonErrorThrower
{
public:
	JsonErrorThrower();

	bool isJsonError() const;
	QString jsonErrorDescription() const;
	void jsonReset();
	int jsonErrorType();
	void errorTypeReset();

	QString jsonErrorString() const;

	inline void JSON_ERROR(const QString& errorDesc, int error_type = 0)
	{
		qDebug() << errorDesc;
		isError = true;
		errorDescription = errorDesc;
		errorType = error_type;
	}
private:
	bool isError;
	QString errorDescription;
	int errorType;
};

}

#endif
