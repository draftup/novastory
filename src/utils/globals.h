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

}

#endif // APIHANDLER_H
