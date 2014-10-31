#ifndef	LOGGEDACTIONS_H
#define LOGGEDACTIONS_H

#include "user.h"

namespace novastory
{

class LoggedActions
{
public:
	int userid() const;
	void setUserID(int userid);
	void setUser(const User& user);
	bool isLogged() const;
protected:
	User m_user;
};

}

#endif
