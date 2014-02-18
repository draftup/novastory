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

	QString jsonErrorString() const;

	inline void JSON_ERROR(const QString& errorDesc)
	{
		qDebug() << errorDesc;
		isError = true;
		errorDescription = errorDesc;
	}
private:
	bool isError;
	QString errorDescription;
};

}

#endif
