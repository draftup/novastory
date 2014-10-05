#ifndef PROFILE_H
#define PROFILE_H

#include "logic/user.h"
#include "htmltemplate.h"

namespace novastory
{

class Profile : public HtmlTemplate
{
public:
	Profile(User* target, User* self);

	QByteArray html() const override;
private:
	User* selfUser;
	User* tragetUser;
};

}

#endif
