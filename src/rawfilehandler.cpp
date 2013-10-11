#include "rawfilehandler.h"
#include <QDebug>
#include <QFile>
#include <QDir>

namespace novastory
{

RawFileHandler::RawFileHandler( QTcpSocket *bindedSocket )
{

}

RawFileHandler::~RawFileHandler()
{

}

QByteArray RawFileHandler::data( const QString& path /*= ""*/ )
{
	const QString filePath = QDir::currentPath() + "/public" + path;
	QFile existFile(filePath);
	qDebug() << "Raw file handler opening: " << filePath;
	if()
}

}
