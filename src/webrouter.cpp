#include "webrouter.h"
#include "rawfilehandler.h"
#include <QTextStream>
#include <QTcpSocket>

namespace novastory
{
	WebRouter::WebRouter( QTcpSocket *bindedSocket ) : WebRequest(bindedSocket)
	{
		appendHandler(new RawFileHandler(bindedSocket));
	}

	QString WebRouter::path() const
	{
		return parsedValues["path"];
	}

	void WebRouter::removeHandler( DataHandler* handler )
	{
		handlers.removeAll(handler);
	}

	void WebRouter::appendHandler( DataHandler* handler )
	{
		handlers.append(handler);
	}

	void WebRouter::sendHtml()
	{
		QTextStream os(bindedSocket);
		os.setAutoDetectUnicode(true);
		os << "HTTP/1.0 200 Ok\r\n"
		   << "Content-Type: text/html; charset=\"utf-8\"\r\n";
		for(DataHandler* handler : handlers)
		{
			os << handler->data(path());
		}
	}

}