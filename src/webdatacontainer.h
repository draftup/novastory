#ifndef WEBDATACONTAINER_H
#define WEBDATACONTAINER_H

#include <QByteArray>
#include <QString>
#include <QDateTime>

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
	WebDataContainer(const QByteArray& other, const QString& mimetype = QString());
	QString toString() const;

	void setMimeType(const QString& mime);
	const QString& mimeType() const;

	void setModificatedDate(const QDateTime& date) { m_modificated = date; };
	const QDateTime& modificatedDate() const { return m_modificated; };

	QString eTag() const;
protected:
	void processImage(const QString& data);
private:
	QString m_mimetype;
	QDateTime m_modificated;
};

}

#endif // WEBDATACONTAINER_H
