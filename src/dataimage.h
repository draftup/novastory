#ifndef DATAIMAGE_H
#define DATAIMAGE_H

#include <QByteArray>
#include <QString>

namespace novastory
{
/**
 * @class	Image container
 *
 * @brief	RFC 2397 Image container
 *
 * @author	Alexey Kasyanchuk
 * @date	16.04.2014
 */

class DataImage : public QByteArray
{
public:
	DataImage(const QString& data);
	const QString& mimeType() const;
	QString toString() const;
protected:
	void processImage(const QString& data);
private:
	QString m_mimetype;
};

}

#endif // APIHANDLER_H
