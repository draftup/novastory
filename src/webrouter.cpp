#include "webrouter.h"
#include <QTextStream>
#include <QTcpSocket>

namespace novastory
{
	WebRouter::WebRouter( QTcpSocket *bindedSocket ) : WebRequest(bindedSocket)
	{

	}

	QString WebRouter::path() const
	{
		return parsedValues["path"];
	}

	void WebRouter::removeHandler( HtmlHandler* handler )
	{
		handlers.removeAll(handler);
	}

	void WebRouter::appendHandler( HtmlHandler* handler )
	{
		handlers.append(handler);
	}

	void WebRouter::sendHtml()
	{
		QTextStream os(bindedSocket);
		os.setAutoDetectUnicode(true);
		os << "HTTP/1.0 200 Ok\r\n"
		   << "Content-Type: text/html; charset=\"utf-8\"\r\n";
		for(HtmlHandler* handler : handlers)
		{
			os << handler->html(path());
		}
	}

}