#ifndef GLOBALS_H
#define GLOBALS_H

#include <QString>

#ifdef QT_NO_DEBUG
#define VERIFY(x) (x)
#else //QT_NO_DEBUG
#define VERIFY(x) Q_ASSERT(x)
#endif //QT_NO_DEBUG

namespace novastory
{

QString md5(const QString& str);
QString sha1(const QString& str);
qint64 unixtime();
void sendMail(const QString& to, const QString& subject, const QString& message);
void sendAsyncMail(const QString& to, const QString& subject, const QString& message);

inline QByteArray htmlHeaderGen(const QString& mimetype, int size)
{
	return ("HTTP/1.1 200 OK\n"
		"Server: novastory\n"
		"Content-Type: " + mimetype + "\n"
		"Content-Length: " + QString::number(size) + "\n\n").toLatin1();
}

}

#endif // APIHANDLER_H
