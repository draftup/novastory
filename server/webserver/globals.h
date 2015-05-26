#ifndef GLOBALS_H
#define GLOBALS_H

#include <QString>
#include <QDateTime>
#include <QTimeZone>
#include "webdatacontainer.h"
#include <QLocale>
#include <QDebug>

class QFile;

#ifdef QT_NO_DEBUG
#define VERIFY(x) (x)
#else //QT_NO_DEBUG
#define VERIFY(x) Q_ASSERT(x)
#endif //QT_NO_DEBUG

#ifdef Q_OS_WIN
#define NVSTR(str) QString::fromLocal8Bit(str)
#else
#define NVSTR(str) QString(str)
#endif

namespace novastory
{
/**
 * @brief	Generate MD5 hashstring from given string
 *
 * @author	Alexey Kasyanchuk
 * @date	04.04.2014
 *
 * @param	str	Input string for md5 generation
 *
 * @return	Generated md5 hash string
 */
QString md5(const QString& str);

/**
 * @brief	Generate SHA1 hashstring from given string
 *
 * @author	Alexey Kasyanchuk
 * @date	04.04.2014
 *
 * @param	str	Input string for sha1 generation
 *
 * @return	Generated sha1 hash string
 */
QString sha1(const QString& str);

/**
 * @brief	Get current unixtime
 *
 * @author	Alexey Kasyanchuk
 * @date	04.04.2014
 *
 * @return	Current unixtime.
 */
qint64 unixtime();

/**
 * @brief	Sends a mail to given email address
 *
 * @author	Alexey Kasyanchuk
 * @date	04.04.2014
 *
 * @param	to			 	Email address for given mail
 * @param	subject		 	Mail subject
 * @param	textofmessage	Sending message
 *
 * @see sendAsyncMail()
 *
 * @note This method work synchronously. Use @ref sendAsyncMail() for async calls.
 */
void sendMail(const QString& to, const QString& subject, const QString& message, const QList<QFile*>& attachments = QList<QFile*>(), bool disable_log = false);

/**
 * @brief	Sends a mail to given email address
 *
 * @author	Alexey Kasyanchuk
 * @date	04.04.2014
 *
 * @param	to			 	Email address for given mail
 * @param	subject		 	Mail subject
 * @param	message			Sending message
 *
 * @see sendMail()
 *
 * @note This method work asynchronously. Use @ref sendMail() for sync calls.
 *
 */
void sendAsyncMail(const QString& to, const QString& subject, const QString& message, const QList<QFile*>& attachments = QList<QFile*>());

inline QString RFC822Date(const QDateTime& time)
{
	QLocale::setDefault(QLocale::c());
	QDateTime rfcTime = time;
	rfcTime = rfcTime.toTimeZone(QTimeZone("Etc/GMT"));
	QString date = QLocale::c().toString(rfcTime, "ddd, dd MMM yyyy HH:mm:ss") + " GMT";
	QLocale::setDefault(QLocale::system());
	return date;
}

QString tr(const QString& key, const QString& dis = QString());
QString translate(const QString& context, const QString& key, const QString& dis = QString());

/**
 * @fn	inline QByteArray htmlHeaderGen(const QString& mimetype, int size)
 *
 * @brief	Generate base html header for file
 *
 * @author	Alexey Kasyanchuk
 * @date	04.04.2014
 *
 * @param	mimetype	The mimetype of file
 * @param	size		The size of file
 * @param	status		Html status (200 by default)
 *
 * @return	Html header. Apeend you body to it.
 */

inline QByteArray htmlHeaderGen(const QString& mimetype = QString(), int size = -1, const QString& status = "200 OK", const QString& additional = QString())
{
	QByteArray responce = (
		"HTTP/1.1 " + status + "\n"
		"Server: novastory\n"
		"Date: " + RFC822Date(QDateTime::currentDateTime()) + "\n"
		+ additional +
		((mimetype.isNull()) ? QString() : "Content-Type: " + mimetype + "\n") +
		((size >= 0) ? "Content-Length: " + QString::number(size) + "\n\r\n" : "\n\r\n")
	).toLatin1();
	qDebug() << "Responce header:";
	qDebug() << responce;
	return responce;
}

inline QByteArray htmlHeaderGen(const WebDataContainer& data, const QString& status = "200 OK", const QString& additional = QString())
{
	QString addAdditional = additional;
	QString etag = data.eTag();
	if (!etag.isEmpty())
	{
		addAdditional += QString("ETag: %1\n").arg(etag);
	}

	if (!data.modificatedDate().isNull())
	{
		addAdditional += QString("Last-Modified: %1\n").arg(RFC822Date(data.modificatedDate()));
	}

	return htmlHeaderGen(data.mimeType(), data.size(), status, addAdditional);
}

inline QByteArray htmlData(const WebDataContainer& data, const QString& status = "200 OK", const QString& additional = QString(), const QHash<QString, QString>& header = QHash<QString, QString>())
{
	// Компресия deflate
	WebDataContainer mData = data;
	QString addAdditional = additional;
	if (header["Accept-Encoding"].contains("deflate"))
	{
		mData = qCompress(mData);
		addAdditional += "Content-Encoding: deflate\n";
	}
	return htmlHeaderGen(mData, status, addAdditional) + mData;
}

inline QByteArray htmlData(const QByteArray& data, const QString& mimetype = QString(), const QString& status = "200 OK", const QString& additional = QString(), const QHash<QString, QString>& header = QHash<QString, QString>())
{
	// Компресия deflate
	QByteArray mData = data;
	QString addAdditional = additional;
	if (header["Accept-Encoding"].contains("deflate"))
	{
		mData = qCompress(mData);
		addAdditional += "Content-Encoding: deflate\n";
	}
	return htmlHeaderGen(mimetype, mData.size(), status, addAdditional) + mData;
}

QString selectorId(const QString& html, const QString& selector);


/// Round up to next higher power of 2 (return x if it's already a power
/// of 2).
inline int
pow2roundup(int x)
{
	if (x < 0)
	{
		return 0;
	}
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;
}


}

#endif // APIHANDLER_H
