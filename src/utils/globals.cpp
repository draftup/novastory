#include "globals.h"

#include <QCryptographicHash>

QString novastory::md5( const QString& str )
{
	return QString(QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Md5).toHex());
}

QString novastory::sha1( const QString& str )
{
	return QString(QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Sha1).toHex());
}
