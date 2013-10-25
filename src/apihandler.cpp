#include "apihandler.h"
#include <QDebug>
#include <QTcpSocket>

namespace novastory
{

ApiHandler::ApiHandler(QTcpSocket* bindedSocket) :
	socket(bindedSocket)
{

}

ApiHandler::~ApiHandler()
{

}

bool ApiHandler::handle(const QString& path)
{
	if (path != "/api")
	{
		return false;
	}

	return true;
}

}
