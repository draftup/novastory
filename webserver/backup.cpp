#include "backup.h"
#include "config.h"
#include <QDebug>
#include <QProcess>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QEventLoop>

namespace novastory
{
void Backup::backupDatabase()
{
	qDebug() << "Backuping" << MYSQL_DATABASE << "database";

	QProcess mysqldump;
	mysqldump.start("mysqldump",
					QStringList()
					<< "-u" << MYSQL_USER
					<< "--password=" MYSQL_PASSWORD
					<< "-h" << MYSQL_HOST
					<< MYSQL_DATABASE
				   );

	if (!mysqldump.waitForStarted())
	{
		qCritical("Something wrong on mysql dump starting");
		return;
	}

	mysqldump.waitForFinished(-1);
	if (mysqldump.exitStatus() != QProcess::NormalExit)
	{
		qCritical("Something wrong on mysql dump exit");
		return;
	}

	QByteArray sql = mysqldump.readAll();


	QNetworkAccessManager manager;
	QNetworkRequest request;
	request.setUrl(QUrl("https://www.googleapis.com/upload/drive/v2/files?uploadType=media"));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
	request.setHeader(QNetworkRequest::ContentLengthHeader, sql.size());
	request.setRawHeader("Authorization", "Bearer ya29.qgFgLffz4XY_XNsHMy9lmgzTTxdVqHPeNBf79xA42P-cGic73fSehFnt1EbVh03YEhU_-WBLU6zSow");

	QNetworkReply* reply = manager.post(request, sql);

	QEventLoop eLoop;
	QObject::connect(reply, &QNetworkReply::finished, [reply, &eLoop]
	{
		qDebug() << reply->readAll();
		eLoop.quit();
	});

	eLoop.exec();

	qDebug() << "Database dumped";

}

}
