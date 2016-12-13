#ifndef WEBSOCKETSLISTENER_H
#define WEBSOCKETSLISTENER_H

#include <QThread>
#include <QObject>
#include <QSharedPointer>
#include <QVariant>
#include "datahandler.h"
class QWebSocketServer;
class QWebSocket;

namespace novastory
{

class WebSocketsListener : public QThread
{
	Q_OBJECT
public:
	WebSocketsListener(quint16 port = 8081, bool secure = false);
	virtual ~WebSocketsListener();
	void run() override;
	void appendHandler(DataHandler* handler)
	{
		m_handlers.append(QSharedPointer<DataHandler>(handler));
	}
	bool broadcastTextMessage(const QString& message, const QString& filter = QString(), const QVariant& filterValue = QVariant());
private slots:
	void onNewWebSocketConnection();
	void processWebSocketTextMessage(QString message);
	void processWebSocketBinaryMessage(QByteArray message);
	void webSocketDisconnected();
private:
	void startServer(quint16 port = 8081, bool secure = false);
	QWebSocketServer* m_pWebSocketServer;
	QList<QWebSocket*> m_pWebSocketClients;
	QList< QSharedPointer<DataHandler> > m_handlers;
};

}

#endif
