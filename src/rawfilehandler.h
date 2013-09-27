#ifndef RAWFILEHANDLER_H
#define RAWFILEHANDLER_H

#include "htmlhandler.h"
class QTcpSocket;

namespace novastory
{

class RawFileHandler : public HtmlHandler
{
public:
	RawFileHandler(QTcpSocket *bindedSocket);
	~RawFileHandler();
};

}

#endif // RAWFILEHANDLER_H
