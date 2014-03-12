#ifndef WEBPROCESS_H
#define WEBPROCESS_H

#include <QRunnable>
#include <QObject>
class QTcpSocket;
#include <QScopedPointer>
#include <QEventLoop>

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
private:
	QScopedPointer<QTcpSocket> socket;
	QScopedPointer<QEventLoop> eventLoop;
	int socketDescriptor;
	static int processCounter;
};

}

#endif
