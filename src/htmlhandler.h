#ifndef HTMLHANDLER_H
#define HTMLHANDLER_H

#include <QString>

namespace novastory
{

class HtmlHandler
{
public:
	virtual QString html(QString path = "");
};

}

#endif
