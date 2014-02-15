#ifndef	USER_H
#define USER_H

#include "sql/sqlizable.h"

class Test_LogicUsers;

namespace novastory
{

class User : protected Sqlizable
{
	friend class ::Test_LogicUsers;
	Q_OBJECT
	Q_PROPERTY(int userid READ userid WRITE setUserID)
	Q_PROPERTY(QString username READ username WRITE setUsername)
	Q_PROPERTY(QString password READ password WRITE setPassword)
	Q_PROPERTY(QString salt READ salt WRITE setSalt)
	Q_PROPERTY(QString email READ email WRITE setEmail)
public:
	User();

	bool addUser();

	int userid() const;
	void setUserID(int userid);

	const QString& username() const;
	void setUsername(const QString& username);

	const QString& password() const;
	void setRawPassword(const QString& password);

	const QString& salt() const;
	void setSalt(const QString& salt);

	const QString& email() const;
	void setEmail(const QString& email);
protected:
	QString generateSalt() const;
	void setPassword(const QString& password);

private:
	int m_userid;
	QString m_username;
	QString m_password;
	QString m_salt;
	QString m_email;
};

}

#endif
