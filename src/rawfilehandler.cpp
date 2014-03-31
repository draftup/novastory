#include "rawfilehandler.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QTcpSocket>

#include <QMimeDatabase>
#include <QMimeType>

namespace novastory
{

RawFileHandler::RawFileHandler(QTcpSocket* bindedSocket) :
	socket(bindedSocket)
{
	// set default directory
	resetDirectory();
}

RawFileHandler::~RawFileHandler()
{

}

bool RawFileHandler::handle(const QString& type, const QString& path, const QHash<QString, QString>& post /* = QHash<QString, QString>() */, const QString& get /* = "" */)
{
	QString filePath = workingDirectory + path;
	
	if(path == "/")
	{
		filePath = workingDirectory + "/index.html";
	}
	
	QFile existFile(filePath);
	if (existFile.open(QIODevice::ReadOnly))
	{
		qDebug() << "Raw file handler: " << filePath;

		QByteArray data = existFile.readAll();

		QMimeDatabase db;
		QMimeType mime = db.mimeTypeForFileNameAndData(existFile.fileName(), data);

		qDebug() << "Raw file type is: " << mime.name();

		socket->write(
			("HTTP/1.1 200 OK\n"
			"Server: novastory\n"
			"Content-Type: " + mime.name() + "\n"
			"Content-Length: " + QString::number(data.size()) + "\n\n").toLatin1()
			);

		socket->write(data);

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

void RawFileHandler::resetDirectory()
{
	QDir dataDirectory = QDir(DATA_DIRECTORY + QString("/public"));
	if (dataDirectory.exists())
	{
		workingDirectory = dataDirectory.absolutePath();
	}
	else if (QDir(QDir::currentPath() + "/../.." + "/public").exists())
	{
		workingDirectory = QDir::currentPath() + "/../.." + "/public";
	}
	else if (QDir(QDir::currentPath() + "/public").exists())
	{
		workingDirectory = QDir::currentPath() + "/public";
	}
	else
	{
		Q_ASSERT(false && "No public directory founded");
	}
}

}
