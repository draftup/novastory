#include "loggedactions.h"

namespace novastory
{

int LoggedActions::userid() const
{
	return m_user.userid();
}

void LoggedActions::setUser(const User& user)
{
	m_user = user;
}

void LoggedActions::setUserID(int userid)
{
	m_user.setUserID(userid);
}

bool LoggedActions::isLogged() const
{
	return m_user.isLogined();
}

}