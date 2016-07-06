#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QString>
#include <QHash>
#include <QString>
#include <QDomDocument>
#include <QDateTime>

class QTcpSocket;
class QWebSocket;

namespace novastory
{

class DataHandler
{
public:
	virtual bool handle(
		QTcpSocket* socket,
		const QString& type,
		const QString& path,
		const QHash<QString, QString>& post = QHash<QString, QString>(),
		const QHash<QString, QString>& get = QHash<QString, QString>(),
		const QHash<QString, QString>& header = QHash<QString, QString>(),
		const QHash<QString, QString>& cookies = QHash<QString, QString>()
	) = 0;

	virtual void webHandle(QWebSocket* webSocket, QList<QWebSocket *>* clients, QString message) {};
	virtual void sitemap(QDomElement& parent_element, QDomDocument& doc) {};
	void appendSitemapElement(
		QDomElement& parent_element,
		QDomDocument& doc,
		const QString& location,
		const QDateTime& lastmod = QDateTime(),
		const QString& changefreq = QString(),
		double priority = -1
	)
	{
		QDomElement url = doc.createElement("url");

		QDomElement loc = doc.createElement("loc");
		loc.appendChild(doc.createTextNode(location));
		url.appendChild(loc);

		if (!lastmod.isNull())
		{
			QDomElement lastmod_el = doc.createElement("lastmod");
			lastmod_el.appendChild(doc.createTextNode(lastmod.toString()));
			url.appendChild(lastmod_el);
		}

		if (!changefreq.isNull())
		{
			QDomElement changefreq_el = doc.createElement("changefreq");
			changefreq_el.appendChild(doc.createTextNode(changefreq));
			url.appendChild(changefreq_el);
		}

		if (priority >= 0.0)
		{
			QDomElement priority_el = doc.createElement("priority");
			priority_el.appendChild(doc.createTextNode(QString::number(priority)));
			url.appendChild(priority_el);
		}

		parent_element.appendChild(url);
	};
};

}

#endif
