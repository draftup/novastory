#ifndef	USER_H
#define USER_H

#include "sql/sqlizable.h"
#include "utils/globals.h"
#include "jsonthrower.h"

class Test_LogicUsers;

namespace novastory
{

class User : protected Sqlizable, public JsonThrower
{
	friend class ::Test_LogicUsers;
	Q_OBJECT
	Q_PROPERTY(int userid READ userid WRITE setUserID RESET resetUserid)
	Q_PROPERTY(QString password READ password WRITE setPassword RESET resetPassword)
	Q_PROPERTY(QString salt READ salt WRITE setSalt RESET resetSalt)
	Q_PROPERTY(QString email READ email WRITE setEmail RESET resetEmail)

	Q_PROPERTY(QString firstname READ firstName WRITE setFirstName RESET resetFirstName)
	Q_PROPERTY(QString lastname READ lastName WRITE setLastName RESET resetLastName)
	Q_PROPERTY(QString nickname READ nickName WRITE setNickName RESET resetNickName)
public:
	User();

	bool addUser();

	int userid() const;
	void setUserID(int userid);
	void resetUserid();

	const QString& password() const;
	void setRawPassword(const QString& password);
	void setSHA1Password(const QString& sha1password);
	void resetPassword();

	bool login(const QString& email, const QString& sha1password);
	bool loginByToken(const QString& email, const QString& token);
	bool loginByToken(const int quserid, const QString& token);
	bool isLogined() const;
	const QString& token() const;

	bool forgotPasswordMessageSend(const QString& email);
	bool confirmPasswordReset(const QString& forgotToken);

	const QString& salt() const;
	void setSalt(const QString& salt);
	void resetSalt();

	const QString& email() const;
	void setEmail(const QString& email);
	void resetEmail();

	bool removeUser();

	static User* verifyUser(const QString& token);

	const QString& firstName() const { return m_firstname; };
	void setFirstName(const QString& name) { m_firstname = name; };
	void resetFirstName(){ m_firstname = QString(); };

	const QString& lastName() const { return m_lastname; };
	void setLastName(const QString& name) { m_lastname = name; };
	void resetLastName(){ m_lastname = QString(); };

	const QString& nickName() const { return m_nickname; };
	void setNickName(const QString& name) { m_nickname = name; };
	void resetNickName(){ m_nickname = QString(); };
protected:
	QString generateSalt() const;
	void setPassword(const QString& password);
	inline QString generatePassword(const QString& sha1password, const QString& password_salt)
	{
		return md5(sha1password + sha1(password_salt) + "novastory");
	}
	inline QString generateToken(qint64 time, int userid, const QString& usersalt)
	{
		return sha1(QString::number(time) + sha1(QString::number(userid) + sha1(usersalt) + sha1("degitx-jelu-leparusvega")));
	}
private:
	int m_userid;
	QString m_password;
	QString m_salt;
	QString m_email;

	QString m_token;

	QString m_firstname;
	QString m_lastname;
	QString m_nickname;
};

}

#endif
