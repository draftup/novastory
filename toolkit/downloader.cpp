#include "downloader.h"
#include <QFile>
#include <QDebug>
#include <QEventLoop>
#include <QDir>
#include <iostream>
#include <windows.h>

namespace novastory
{

Downloader::Downloader(const QUrl& file, const QString& destination) :
	QObject(), m_url(file), m_destination(destination)
{
	HANDLE consolehwnd = GetStdHandle(STD_OUTPUT_HANDLE);

	m_output.setFileName(QDir(m_destination).absolutePath() + "/" + m_url.fileName());
	if (!m_output.open(QIODevice::ReadWrite | QIODevice::Truncate))
	{
		SetConsoleTextAttribute(consolehwnd, FOREGROUND_RED);
		qFatal("File open for download failed");
	}

	SetConsoleTextAttribute(consolehwnd, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	QNetworkRequest request(m_url);
	QNetworkReply* currentDownload = m_WebCtrl.get(request);
	connect(currentDownload, SIGNAL(downloadProgress(qint64, qint64)),
			SLOT(downloadProgress(qint64, qint64)));
	connect(currentDownload, SIGNAL(finished()),
			SLOT(fileDownloaded()));
	connect(currentDownload, SIGNAL(readyRead()),
		SLOT(downloadReadyRead()));
}

Downloader::~Downloader()
{
	m_output.close();
}

void Downloader::fileDownloaded()
{
	m_output.flush();
	std::cout << std::endl;
	HANDLE consolehwnd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consolehwnd, FOREGROUND_GREEN);
	std::cout << "File downloaded" << std::endl;
	emit downloaded();
}

void Downloader::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	std::cout << '\r' << "Downloading file: " << (int)((double)bytesReceived * 100 / bytesTotal) << "%" << std::flush;
}

void Downloader::download(QString url, QString destination)
{
	Downloader dw(QUrl(url), destination);
	dw.wait();
}

void Downloader::wait() const
{
	QEventLoop loop;
	QObject::connect(this, SIGNAL(downloaded()), &loop, SLOT(quit()));
	loop.exec();
}

void Downloader::downloadReadyRead()
{
	QNetworkReply* reply = (QNetworkReply*)sender();
	Q_ASSERT(reply != nullptr);
	m_output.write(reply->readAll());
}

QByteArray Downloader::data()
{
	m_output.seek(0);
	return m_output.readAll();
}

void Downloader::close()
{
	m_output.close();
}

}