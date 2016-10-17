#include "websocketslistener.h"
#include "datahandler.h"
#include <QWebSocketServer>
#include <QWebSocket>

namespace novastory
{

WebSocketsListener::WebSocketsListener() : m_pWebSocketServer(nullptr)
{
	setObjectName("Web Sockets Thread");
	startServer();
}

WebSocketsListener::~WebSocketsListener()
{
	if (m_pWebSocketServer != nullptr)
	{
		m_pWebSocketServer->close();
		m_pWebSocketServer->deleteLater();
		qDeleteAll(m_pWebSocketClients.begin(), m_pWebSocketClients.end());
	}
}

void WebSocketsListener::run()
{
	exec();
}

void WebSocketsListener::broadcastTextMessage(const QString& message, const QString& filter /* = QString() */, const QVariant& filterValue /* = QString() */)
{
	qDebug() << "Broadcasting WebSockets message" << message.left(1024) + (message.size() > 1024 ? "..." : "") << "(size: " + QString::number(message.size()) + ")";
	for (QWebSocket* socket : m_pWebSocketClients)
	{
		// отправка только определенным участникам в фильтре
		if (!filter.isNull() && !filter.isEmpty())
		{
			QVariant testValue = socket->property(filter.toLatin1().constData());
			if (testValue.isNull())
			{
				continue;
			}

			if (!filterValue.isNull() && testValue != filterValue)
			{
				continue;
			}
		}

		socket->sendTextMessage(message);
	}
}

void WebSocketsListener::startServer()
{
	m_pWebSocketServer = new QWebSocketServer(QStringLiteral("Echo Server"),
			QWebSocketServer::NonSecureMode);
	if (m_pWebSocketServer->listen(QHostAddress::Any, 8081))
	{
		qDebug() << "WebSocket server listening on port" << 8081;
		connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
				this, &WebSocketsListener::onNewWebSocketConnection);
	}
	else
	{
		qCritical() << "Can't run WebSocket server";
	}
}

void WebSocketsListener::onNewWebSocketConnection()
{
	qDebug() << "WebSocket new connection.";
	QWebSocket* pSocket = m_pWebSocketServer->nextPendingConnection();

	connect(pSocket, &QWebSocket::textMessageReceived, this, &WebSocketsListener::processWebSocketTextMessage);
	connect(pSocket, &QWebSocket::binaryMessageReceived, this, &WebSocketsListener::processWebSocketBinaryMessage);
	connect(pSocket, &QWebSocket::disconnected, this, &WebSocketsListener::webSocketDisconnected);

	m_pWebSocketClients << pSocket;
}

void WebSocketsListener::processWebSocketTextMessage(QString message)
{
	QWebSocket* pClient = qobject_cast<QWebSocket*>(sender());
	qDebug() << "Web Socket message received:" << message.left(1024) + (message.size() > 1024 ? "..." : "") << "(size: " + QString::number(message.size()) + ")";
	if (pClient)
	{
		//pClient->sendTextMessage(message);
		for (QSharedPointer<DataHandler> handler : m_handlers)
		{
			handler->webHandle(pClient, &m_pWebSocketClients, message);
		}
	}
}

void WebSocketsListener::processWebSocketBinaryMessage(QByteArray message)
{
	QWebSocket* pClient = qobject_cast<QWebSocket*>(sender());
	qDebug() << "Web Socket binary message received:" << message;
	if (pClient)
	{
		//pClient->sendBinaryMessage(message);
	}
}

void WebSocketsListener::webSocketDisconnected()
{
	QWebSocket* pClient = qobject_cast<QWebSocket*>(sender());
	if (pClient)
	{
		m_pWebSocketClients.removeAll(pClient);
		pClient->deleteLater();
	}
	qDebug() << "Web Socket disconnected:" << pClient << "(now:" << m_pWebSocketClients.size() << ")";
}

}