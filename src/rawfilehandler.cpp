#include "rawfilehandler.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QTcpSocket>
namespace novastory
{

RawFileHandler::RawFileHandler(QTcpSocket* bindedSocket) :
	socket(bindedSocket),
	workingDirectory(QDir::currentPath() + "/../.." + "/public")
{

}

RawFileHandler::~RawFileHandler()
{

}

bool RawFileHandler::handle(const QString& type, const QString& path, const QHash<QString, QString>& post /* = QHash<QString, QString>() */, const QString& get /* = "" */)
{
	const QString filePath = workingDirectory + path;
	QFile existFile(filePath);
	if (existFile.open(QIODevice::ReadOnly))
	{
		qDebug() << "Raw file handler: " << filePath;
		socket->write(existFile.readAll());

		return true;
	}

	return false;
}

void RawFileHandler::setDirectory(const QString& path)
{
	QDir targetDirectory(path);
	if (targetDirectory.exists())
	{
		workingDirectory = targetDirectory.absolutePath();
	}
}

QString RawFileHandler::directory() const
{
	return workingDirectory;
}

}
