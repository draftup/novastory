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
	Q_PROPERTY(int userid READ userid WRITE setUserID RESET resetUserid)
	Q_PROPERTY(QString username READ username WRITE setUsername RESET resetUsername)
	Q_PROPERTY(QString password READ password WRITE setPassword RESET resetPassword)
	Q_PROPERTY(QString salt READ salt WRITE setSalt RESET resetSalt)
	Q_PROPERTY(QString email READ email WRITE setEmail RESET resetEmail)
public:
	User();

	bool addUser();

	int userid() const;
	void setUserID(int userid);
	void resetUserid();

	const QString& username() const;
	void setUsername(const QString& username);
	void resetUsername();

	const QString& password() const;
	void setRawPassword(const QString& password);
	void resetPassword();

	const QString& salt() const;
	void setSalt(const QString& salt);
	void resetSalt();

	const QString& email() const;
	void setEmail(const QString& email);
	void resetEmail();

	static User* verifyUser(const QString& token);
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
