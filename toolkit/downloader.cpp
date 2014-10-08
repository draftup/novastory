#include "downloader.h"
#include <QFile>
#include <QDebug>
#include <QEventLoop>

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
		QFile file(m_destination + "/" + m_url.fileName());
		if (file.open(QIODevice::WriteOnly)) 
		{
			file.write(reply->readAll());
			file.close();
		}
		else
		{
			qFatal("Failed to save file");
		}

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

}