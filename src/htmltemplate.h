#ifndef HTMLTEMPLATE_H
#define HTMLTEMPLATE_H

#include <QByteArray>

namespace novastory
{
class HtmlTemplate
{
public:
	virtual QByteArray html() const = 0;
};
}

#endif //HTMLTEMPLATE_H