#ifndef WEBPROCESS_H
#define WEBPROCESS_H

#include <QRunnable>
#include <QObject>
class QTcpSocket;
#include <QScopedPointer>
#include <QEventLoop>
#include <QTimer>
#include <atomic>

namespace novastory
{

class WebProcess : public QObject, public QRunnable
{
	Q_OBJECT
public:
	WebProcess(int socket_descriptor);
	~WebProcess();
	void run() override;
private slots:
	void showHtmlPage();
	void onSocketClosed();
	void onBytesWriten(qint64 bytes);
	void closedByInterval();
	void onSocketDisconnected();
private:
	void closeSocket();
	QScopedPointer<QTcpSocket> socket;
	QScopedPointer<QEventLoop> eventLoop;
	QScopedPointer<QTimer> timeout;
	int socketDescriptor;
	static std::atomic<int> processCounter;
};

}

#endif
