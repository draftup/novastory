#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QString>

namespace novastory
{

class DataHandler
{
public:
	virtual QByteArray data(const QString& path = "") = 0;
};

}

#endif
