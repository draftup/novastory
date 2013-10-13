#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QString>
#include <QMimeType>

namespace novastory
{

class DataHandler
{
public:
	virtual QByteArray data(const QString& path = "") = 0;
	virtual QMimeType mimeType() const = 0;
};

}

#endif
