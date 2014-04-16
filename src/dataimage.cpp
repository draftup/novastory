#include "dataimage.h"
#include <QRegExp>
#include <QDebug>

void novastory::DataImage::processImage( const QString& data )
{
	QRegExp rx("data:(.*);base64,(.*)");
	if(rx.indexIn(data) >= 0)
	{
		m_mimetype = rx.cap(1);
		QByteArray::operator=(fromBase64(rx.cap(2).toLatin1()));
	}
}

const QString& novastory::DataImage::mimeType() const
{
	return m_mimetype;
}

novastory::DataImage::DataImage( const QString& data ) : QByteArray()
{
	processImage(data);
}

QString novastory::DataImage::toString() const
{
	return QString("data:%1;base64,%2").arg(m_mimetype).arg(QString::fromLatin1(toBase64()));
}
