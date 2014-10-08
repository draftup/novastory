#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>

namespace novastory
{
	class Downloader : public QObject
	{
		Q_OBJECT
	public:
		explicit Downloader(const QUrl& file, const QString& destination);
		virtual ~Downloader();

		QByteArray downloadedData() const;

		static void download(QString url, QString destination);
	signals:
		void downloaded();
	private slots:
		void fileDownloaded();
		void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	private:
		QNetworkAccessManager m_WebCtrl;
		QUrl m_url;
		QString m_destination;
	};
}

#endif