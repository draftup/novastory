#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QString>
#include <QHash>
#include <QString>

namespace novastory
{

class DataHandler
{
public:
	virtual bool handle(const QString& type, const QString& path, const QHash<QString, QString>& post = QHash<QString, QString>(), const QString& get = "") = 0;
};

}

#endif
