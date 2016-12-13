#include "websocketslistener.h"
#include "datahandler.h"
#include <QWebSocketServer>
#include <QWebSocket>
#include <QFile>
#include <QSslCertificate>
#include <QSslKey>
#include <QDir>
#include "globals.h"

namespace novastory
{

WebSocketsListener::WebSocketsListener(quint16 port, bool secure) : m_pWebSocketServer(nullptr)
{
	setObjectName("Web Sockets Thread");
	startServer(port, secure);
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

bool WebSocketsListener::broadcastTextMessage(const QString& message, const QString& filter /* = QString() */, const QVariant& filterValue /* = QString() */)
{
	qDebug() << "Broadcasting WebSockets message" << message.left(1024) + (message.size() > 1024 ? "..." : "") << "(size: " + QString::number(message.size()) + ")";
	bool sended_once = false;
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
		sended_once = true;
	}

	return sended_once;
}

void WebSocketsListener::startServer(quint16 port, bool secure)
{
#ifdef QT_DEBUG
	QWebSocketServer::SslMode sslMode = QWebSocketServer::NonSecureMode;
#else
	QWebSocketServer::SslMode sslMode = secure ? QWebSocketServer::SecureMode : QWebSocketServer::NonSecureMode;
#endif
	m_pWebSocketServer = new QWebSocketServer(QStringLiteral("Draftup Server"), sslMode);

#ifndef QT_DEBUG
	if (secure)
	{
		// Detect cert
		QString certPath;
		QString keyPath;
		if (QDir(DATA_DIRECTORY + QString("/certs")).exists())
		{
			certPath = QDir(DATA_DIRECTORY + QString("/certs")).absolutePath() + "/draftup.org.crt";
			keyPath = QDir(DATA_DIRECTORY + QString("/certs")).absolutePath() + "/draftup.org.key";
		}
		else if (QDir(SOURCE_DIRECTORY + QString("/certs")).exists())
		{
			certPath = QDir(SOURCE_DIRECTORY + QString("/certs")).absolutePath() + "/draftup.org.crt";
			keyPath = QDir(SOURCE_DIRECTORY + QString("/certs")).absolutePath() + "/draftup.org.key";
		}

		qDebug() << "Certificate file using:" << certPath;
		qDebug() << "Certificate key file using:" << keyPath;

		// SSL Settings
		QSslConfiguration sslConfiguration;
		QFile certFile(certPath);
		QFile keyFile(keyPath);
		VERIFY(certFile.open(QIODevice::ReadOnly));
		VERIFY(keyFile.open(QIODevice::ReadOnly));
		QSslCertificate certificate(&certFile, QSsl::Pem);
		QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);
		certFile.close();
		keyFile.close();
		sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
		sslConfiguration.setLocalCertificate(certificate);
		sslConfiguration.setPrivateKey(sslKey);
		sslConfiguration.setProtocol(QSsl::TlsV1SslV3);
		m_pWebSocketServer->setSslConfiguration(sslConfiguration);
	}
#endif

	if (m_pWebSocketServer->listen(QHostAddress::Any, port))
	{
		qDebug() << "WebSocket" << (secure ? "secure" : "unsecure") << "server listening on port" << port;
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