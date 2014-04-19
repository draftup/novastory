#ifndef WEBDATACONTAINER_H
#define WEBDATACONTAINER_H

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

class WebDataContainer : public QByteArray
{
public:
	WebDataContainer();
	WebDataContainer(const QString& rfc2397data);
	WebDataContainer(const QByteArray & other, const QString& mimetype = QString());
	const QString& mimeType() const;
	QString toString() const;
	void setMimeType(const QString& mime);
protected:
	void processImage(const QString& data);
private:
	QString m_mimetype;
};

}

#endif // WEBDATACONTAINER_H
