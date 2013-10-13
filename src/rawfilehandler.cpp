#include "rawfilehandler.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QMimeDatabase>

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
	if(existFile.open(QIODevice::ReadOnly))
	{
		QByteArray fileData = existFile.readAll();
		QMimeDatabase mimeBase;
		QMimeType mimeType = mimeBase.mimeTypeForData(fileData);
		qDebug() << mimeType.name();
	}

	return QByteArray();
}

QMimeType RawFileHandler::mimeType() const 
{
	return QMimeType();
}

}
