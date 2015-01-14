#include "downloader.h"
#include <QFile>
#include <QDebug>
#include <QEventLoop>
#include <QDir>

namespace novastory
{

Downloader::Downloader(const QUrl& file, const QString& destination) :
	QObject(), m_url(file), m_destination(destination)
{
	QNetworkRequest request(m_url);
	QNetworkReply* currentDownload = m_WebCtrl.get(request);
	connect(currentDownload, SIGNAL(downloadProgress(qint64, qint64)),
			SLOT(downloadProgress(qint64, qint64)));
	connect(currentDownload, SIGNAL(finished()),
			SLOT(fileDownloaded()));
}

Downloader::~Downloader()
{

}

void Downloader::fileDownloaded()
{
	QNetworkReply* reply = (QNetworkReply*)sender();

	QFile file(QDir(m_destination).absolutePath() + "/" + m_url.fileName());
	m_data = reply->readAll();
	if (file.open(QIODevice::WriteOnly))
	{
		file.write(m_data);
		file.close();
	}
	else
	{
		qFatal("Failed to save file");
	}

	qDebug() << m_url.fileName() << "downloaded";

	reply->deleteLater();
	emit downloaded();
}

void Downloader::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{

}

void Downloader::download(QString url, QString destination)
{
	QEventLoop loop;
	Downloader dw(QUrl(url), destination);
	QObject::connect(&dw, SIGNAL(downloaded()), &loop, SLOT(quit()));
	loop.exec();
}

const QByteArray& Downloader::data() const
{
	return m_data;
}

void Downloader::wait() const
{
	QEventLoop loop;
	QObject::connect(this, SIGNAL(downloaded()), &loop, SLOT(quit()));
	loop.exec();
}

}