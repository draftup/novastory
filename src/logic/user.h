#ifndef	USER_H
#define USER_H

#include "sql/sqlizable.h"
#include "utils/globals.h"

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
	void setSHA1Password(const QString& sha1password);
	void resetPassword();

	bool login(const QString& email, const QString& sha1password);
	bool isLogined() const;
	const QString& token();

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
	inline QString generatePassword(const QString& sha1password, const QString& password_salt)
	{
		return md5(sha1password + sha1(password_salt) + "novastory");
	}
	inline QString generateToken(qint64 time, int userid, const QString& usersalt)
	{
		return sha1(time + sha1(userid + sha1(usersalt) + sha1("degitx-jelu-leparusvega")));
	}
private:
	int m_userid;
	QString m_username;
	QString m_password;
	QString m_salt;
	QString m_email;

	QString m_token;
};

}

#endif
