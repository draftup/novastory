#include "webrouter.h"

namespace novastory
{
	WebRouter::WebRouter( QTcpSocket *bindedSocket ) : WebRequest(bindedSocket)
	{

	}

	QString WebRouter::path() const
	{
		return parsedValues["path"];
	}

}