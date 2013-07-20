#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <QTcpServer>

class WebServer : public QTcpServer
{
    Q_OBJECT

public:
    WebServer(QObject *parent = 0);
	virtual ~WebServer();

private slots:
	void someNewConnection();
	void showHtmlPage();

};

#endif
