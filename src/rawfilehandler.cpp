#include "rawfilehandler.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QTcpSocket>
namespace novastory
{

RawFileHandler::RawFileHandler( QTcpSocket *bindedSocket ) : socket(bindedSocket)
{

}

RawFileHandler::~RawFileHandler()
{

}

void RawFileHandler::handle(const QString& path)
{
	const QString filePath = QDir::currentPath() + "/public" + path;
	QFile existFile(filePath);
	qDebug() << "Raw file handler opening: " << filePath;
	if(existFile.open(QIODevice::ReadOnly))
	{
		socket->write(existFile.readAll());
	}
}

}
