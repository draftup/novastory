#include "user.h"
#include <QVariant>
#include <QDebug>
#include <QSqlRecord>
#include <QSqlError>
#include "utils/globals.h"
#include "sql/sqlquery.h"
#include "logic/captcha.h"

novastory::User::User() : m_userid(-1), m_subscriptions_filled(false), m_subscribed_filled(false)
{
	setObjectName("users");
	setProperty("auto_increment", QVariant("userid"));
	setProperty("primary_key", QVariant("userid"));
	setProperty("hidden", QVariant("salt,password"));
}

int novastory::User::userid() const
{
	return m_userid;
}

void novastory::User::setUserID(int userid)
{
	m_userid = userid;
}

void novastory::User::resetUserid()
{
	m_userid = -1;
}

const QString& novastory::User::password() const
{
	return m_password;
}

void novastory::User::setPassword(const QString& password)
{
	m_password = password;
}

void novastory::User::resetPassword()
{
	m_password = QString();
}

const QString& novastory::User::salt() const
{
	return m_salt;
}

void novastory::User::setSalt(const QString& salt)
{
	m_salt = salt;
}

void novastory::User::resetSalt()
{
	m_salt = QString();
}


const QString& novastory::User::email() const
{
	return m_email;
}

void novastory::User::setEmail(const QString& email)
{
	m_email = email;
}

void novastory::User::resetEmail()
{
	m_email = QString();
}

void novastory::User::setRawPassword(const QString& password)
{
	QString currentSalt = salt();

	if (currentSalt.isEmpty())
	{
		// if old salt is empty we must generate new salt
		currentSalt = generateSalt();
		setSalt(currentSalt);
	}

	setPassword(generatePassword(sha1(password), currentSalt));
}

bool novastory::User::addUser()
{
	if (m_password.isEmpty() || m_salt.isEmpty() || m_email.isEmpty())
	{
		JSON_ERROR("Email or password or salt are not valid", 1);
		return false;    // something is empty
	}

	// Checking if user already exist
	SqlQuery query;
	query.prepare("SELECT userid FROM users WHERE email = ?");
	query.bindValue(0, m_email);
	VERIFY(query.exec());

	if (query.size() != 0)
	{
		JSON_ERROR("User " + m_email + " already in database", 2);
		return false;
	}

	bool status = insertSQL();
	if (status)
	{
		// Redmine hook
		SqlQuery redmine_query;
		redmine_query.prepare("INSERT INTO redmine.users(`login`,`hashed_password`,`salt`,`mail`,`admin`,`status`,`language`,`type`,`mail_notification`,`created_on`,`updated_on`) VALUES(:login, :password, :salt, :mail, '0', '1', 'en','User','only_my_events', NOW(), NOW())");
		redmine_query.bindValue(":login", m_email);
		redmine_query.bindValue(":password", m_password);
		redmine_query.bindValue(":salt", m_salt);
		redmine_query.bindValue(":mail", m_email);
		if (redmine_query.exec())
		{
			qDebug() << "Also inserted redmine user";
		}
		else
		{
			qDebug() << "No redmine db, no insert required";
		}
	}

	return status;
}

QString novastory::User::generateSalt() const
{
	return md5(QString::number(unixtime()) + "salt" + (rand() % 256));
}

novastory::User* novastory::User::verifyUser(const QString& token)
{
	Captcha capthaCheck;
	capthaCheck.setToken(token);
	if (!capthaCheck.syncByToken())
	{
		qDebug() << "No userverification for token: " << token;
		return nullptr;
	}
	User* newUser = new User;
	newUser->setPassword(capthaCheck.password());
	newUser->setSalt(capthaCheck.salt());
	newUser->setEmail(capthaCheck.email());

	VERIFY(capthaCheck.deleteByEmail());

	if (!newUser->addUser())
	{
		qCritical() << "User cannot added after captcha check. Something wrong";
		return nullptr;
	}

	qDebug() << "Account" << newUser->email() << "is now activated.";

	return newUser;
}

void novastory::User::setSHA1Password(const QString& sha1password)
{
	QString currentSalt = salt();

	if (currentSalt.isEmpty())
	{
		// if old salt is empty we must generate new salt
		currentSalt = generateSalt();
		setSalt(currentSalt);
	}

	setPassword(generatePassword(sha1password, currentSalt));
}

bool novastory::User::login(const QString& semail, const QString& sha1password)
{
	if (semail.isEmpty() || sha1password.isEmpty())
	{
		JSON_ERROR("Email or password are not valid", 1);
		return false;
	}

	setEmail(semail);

	SqlQuery query;
	query.prepare("SELECT * FROM users WHERE email = :email");
	query.bindValue(":email", email());

	VERIFY(query.exec());

	if (query.size() != 1)
	{
		JSON_ERROR("User with such email not founded: " + email(), 2);
		return false;
	}

	VERIFY(query.next());

	int saltNo = query.record().indexOf("salt");
	QString qsalt = query.value(saltNo).toString();
	int passwordNo = query.record().indexOf("password");
	QString qpassword = query.value(passwordNo).toString();
	int useridNo = query.record().indexOf("userid");
	int quserid = query.value(useridNo).toInt();

	if (qpassword != generatePassword(sha1password, qsalt))
	{
		JSON_ERROR("Password for user " + semail + " was wrong", 3);
		return false;
	}

	Q_ASSERT(quserid > 0);

	qint64 current_time = unixtime();
	m_token = QString::number(current_time) + QString("-") + generateToken(current_time, quserid, qsalt);

	JSON_INSERT("logined", true);
	JSON_INSERT("email", semail);
	JSON_INSERT("userid", quserid);
	JSON_INSERT("token", m_token);

	query.seek(-1);
	return syncProcess(query);
}

bool novastory::User::loginByToken(const QString& semail, const QString& token)
{
	if (semail.isEmpty() || token.isEmpty())
	{
		JSON_ERROR("token or email are not valid", 1);
		return false;
	}

	QStringList tokenList = token.split("-");
	if (tokenList.size() != 2)
	{
		JSON_ERROR("Not valid token", 3);
		return false;
	}

	QString tokenDate = tokenList[0];
	QString tokenKey = tokenList[1];

	setEmail(semail);

	SqlQuery query;
	query.prepare("SELECT * FROM users WHERE email = :email");
	query.bindValue(":email", email());

	VERIFY(query.exec());

	if (query.size() != 1)
	{
		JSON_ERROR("User with such email not founded: " + email(), 2);
		return false;
	}

	VERIFY(query.next());

	int saltNo = query.record().indexOf("salt");
	QString qsalt = query.value(saltNo).toString();
	int useridNo = query.record().indexOf("userid");
	int quserid = query.value(useridNo).toInt();

	if (sha1(tokenDate + sha1(QString::number(quserid) + sha1(qsalt) + sha1("degitx-jelu-leparusvega"))) != tokenKey)
	{
		JSON_ERROR("Token for user " + semail + " was wrong", 4);
		return false;
	}

	m_token = token;

	JSON_INSERT("logined", true);
	JSON_INSERT("userid", quserid);
	JSON_INSERT("email", semail);
	JSON_INSERT("token", m_token);

	query.seek(-1);
	return syncProcess(query);
}

bool novastory::User::loginByToken(const int quserid, const QString& token)
{
	if (quserid <= 0 || token.isEmpty())
	{
		JSON_ERROR("token or userid are not valid", 1);
		return false;
	}

	QStringList tokenList = token.split("-");
	if (tokenList.size() != 2)
	{
		JSON_ERROR("Not valid token", 3);
		return false;
	}

	QString tokenDate = tokenList[0];
	QString tokenKey = tokenList[1];

	setUserID(quserid);

	SqlQuery query;
	query.prepare("SELECT * FROM users WHERE userid = :userid");
	query.bindValue(":userid", userid());

	VERIFY(query.exec());

	if (query.size() != 1)
	{
		JSON_ERROR(QString("User with such userid not founded: ") + quserid, 2);
		return false;
	}

	VERIFY(query.next());

	int saltNo = query.record().indexOf("salt");
	QString qsalt = query.value(saltNo).toString();
	int emailNo = query.record().indexOf("email");
	QString qemail = query.value(emailNo).toString();

	if (sha1(tokenDate + sha1(QString::number(quserid) + sha1(qsalt) + sha1("degitx-jelu-leparusvega"))) != tokenKey)
	{
		JSON_ERROR(QString("Token for user ") + QString::number(quserid) + " was wrong", 4);
		return false;
	}

	m_token = token;

	JSON_INSERT("logined", true);
	JSON_INSERT("userid", quserid);
	JSON_INSERT("email", qemail);
	JSON_INSERT("token", m_token);

	query.seek(-1);
	return syncProcess(query);
}

bool novastory::User::isLogined() const
{
	return !m_token.isEmpty();
}

const QString& novastory::User::token() const
{
	return m_token;
}

bool novastory::User::removeUser()
{
	if (m_userid > 0)
	{
		SqlQuery queryRemoveTest;
		queryRemoveTest.prepare("SELECT * FROM users WHERE userid = ?");
		queryRemoveTest.bindValue(0, m_userid);
		VERIFY(queryRemoveTest.exec());
		if (queryRemoveTest.size() != 1)
		{
			return false;    // noting to delete
		}

		VERIFY(queryRemoveTest.next());
		QString qemail = queryRemoveTest.value("email").toString();

		bool status = removeSQL("userid");
		if (status)
		{
			Q_ASSERT(qemail.size() > 0);
			SqlQuery redmine_query;
			redmine_query.prepare("DELETE FROM redmine.users WHERE mail = ?");
			redmine_query.bindValue(0, qemail);
			if (redmine_query.exec())
			{
				qDebug() << "Removed user from redmine";
			}
			else
			{
				qDebug() << "Redmine db didn't find. Nobody cares.";
			}
		}

		return status;
	}

	if (!m_email.isEmpty())
	{
		SqlQuery queryRemoveTest;
		queryRemoveTest.prepare("SELECT * FROM users WHERE email = ?");
		QString oldemail = m_email;
		queryRemoveTest.bindValue(0, oldemail);
		VERIFY(queryRemoveTest.exec());
		if (queryRemoveTest.size() != 1)
		{
			return false;    // noting to delete
		}

		bool status = removeSQL("email");
		if (status)
		{
			SqlQuery redmine_query;
			redmine_query.prepare("DELETE FROM redmine.users WHERE mail = ?");
			redmine_query.bindValue(0, oldemail);
			if (redmine_query.exec())
			{
				qDebug() << "Removed user from redmine";
			}
			else
			{
				qDebug() << "Redmine db didn't find. Nobody cares.";
			}
		}
		return status;
	}

	return false;
}

bool novastory::User::forgotPasswordMessageSend(const QString& uemail)
{
	if (uemail.isEmpty())
	{
		JSON_ERROR("email are not valid", 1);
		return false;
	}

	SqlQuery query;
	query.prepare("SELECT userid, salt, password FROM users WHERE email = ?");
	query.bindValue(0, uemail);
	VERIFY(query.exec());
	if (query.size() != 1)
	{
		JSON_ERROR("no such user", 2);
		return false;    // noting to delete
	}

	VERIFY(query.next());

	int quserid = query.value("userid").toInt();
	QString salt = query.value("salt").toString();
	QString password = query.value("password").toString();

	SqlQuery queryFCreation;
	queryFCreation.prepare("INSERT INTO userspassforgot VALUES(:token, :userid, :newpassword)");
	QString token = md5(password);
	queryFCreation.bindValue(":token", token);
	queryFCreation.bindValue(":userid", quserid);

	char newpass[9];
	for (int i = 0; i < 8 ; ++i)
	{
		newpass[i] = 'a' + rand() % ('Z' - 'a');
	}
	newpass[8] = '\0';

	queryFCreation.bindValue(":newpassword", generatePassword(sha1(newpass), salt));

	if (!queryFCreation.exec())
	{
		JSON_ERROR("Duplication, you already recive mail", 3);
		return false;    // noting to delete
	}

	qDebug() << "Sending request for user " << uemail << "to reset password";

	setEmail(uemail);
	setUserID(quserid);

	sendAsyncMail(
		m_email,
		"Forgot password notification (novastory.org)",
		"To reset your password click link:\nhttp://novastory.org/api/resetpassword/" + token + "\n\n"
		"Your new password will be: " + newpass + "\n\n"
		"Next time don't foget it :)."
	);

	JSON_INSERT("sended", true);

	return true;
}

bool novastory::User::confirmPasswordReset(const QString& forgotToken)
{
	if (forgotToken.size() != 32)
	{
		JSON_ERROR("wrong token", 1);
		return false;
	}

	SqlQuery queryF;
	queryF.prepare("SELECT * FROM userspassforgot WHERE token = :token");
	queryF.bindValue(":token", forgotToken);
	VERIFY(queryF.exec());

	if (queryF.size() != 1)
	{
		JSON_ERROR("no such token reset", 2);
		return false;
	}

	VERIFY(queryF.next());

	int quserid = queryF.value("userid").toInt();
	QString newpassword = queryF.value("newpassword").toString();

	SqlQuery queryU;
	queryU.prepare("UPDATE users SET password = :password WHERE userid = :userid");
	queryU.bindValue(":password", newpassword);
	queryU.bindValue(":userid", quserid);

	bool status = queryU.exec();
	if (status)
	{
		qDebug() << "Password succesfully reseted to new for user " << quserid;
		setUserID(quserid);

		SqlQuery queryClean;
		queryClean.prepare("DELETE FROM userspassforgot WHERE token = :token");
		queryClean.bindValue(":token", forgotToken);
		VERIFY(queryClean.exec());

		// Redmine hook
		SqlQuery redmine_help_query("SELECT email FROM users WHERE userid = " + QString::number(quserid));
		Q_ASSERT(redmine_help_query.size() == 1);
		VERIFY(redmine_help_query.next());

		SqlQuery redmine_query;
		redmine_query.prepare("UPDATE redmine.users SET hashed_password = :password WHERE mail = :email");
		redmine_query.bindValue(":password", newpassword);
		redmine_query.bindValue(":email", redmine_help_query.value("email").toString());
		if (redmine_query.exec())
		{
			qDebug() << "Removed user from redmine";
		}
		else
		{
			qDebug() << "Redmine db didn't find. Nobody cares.";
		}
	}

	JSON_INSERT("reseted", true);

	return status;
}

bool novastory::User::update()
{
	if (!isLogined())
	{
		JSON_ERROR("not loggined", 1);
		return false;
	}

	if (!updateSQL("userid", QList<QString>() << "salt" << "password" << "email"))
	{
		JSON_ERROR("failed to update", 2);
		return false;
	}

	// Redmine hook
	SqlQuery redmine_query;
	redmine_query.prepare("UPDATE redmine.users SET firstname = :firstname, lastname = :lastname, updated_on = NOW()  WHERE mail = :email");
	redmine_query.bindValue(":firstname", firstName());
	redmine_query.bindValue(":lastname", lastName());
	redmine_query.bindValue(":email", email());
	if (redmine_query.exec())
	{
		qDebug() << "Updated user from redmine";
	}
	else
	{
		qDebug() << "Redmine db didn't find. Nobody cares.";
	}

	return true;
}

void novastory::User::appendProfileJson()
{
	jsonReset();

	if (!isLogined())
	{
		JSON_ERROR("not loggined", 1);
		return;
	}

	JSON_INSERT("user", jsonObject());
}

bool novastory::User::sync()
{
	if (userid() > 0)
	{
		return syncSQL("userid");
	}

	if (!email().isEmpty())
	{
		return syncSQL("email");
	}

	if (!profileId().isEmpty())
	{
		return syncSQL("profileid");
	}

	return false;
}

bool novastory::User::subscribe( const User& targetUser )
{
	if(!isLogined())
	{
		JSON_ERROR("not loggined", 1);
		return false;
	}

	int uid = userid();
	int targetid = targetUser.userid();
	if(targetid <= 0)
	{
		JSON_ERROR("target userid not founded", 2);
		return false;
	}

	if(uid == targetid)
	{
		JSON_ERROR("Cannot subscribe to youself", 4);
		return false;
	}

	SqlQuery query(QString("INSERT INTO subscriptions(userid, targetid) VALUES('%1', '%2')").arg(uid).arg(targetid));
	bool isOk = (query.lastError().type() == QSqlError::NoError);
	if(!isOk)
	{
		JSON_ERROR("Error on subscription", 3);
	}
	return isOk;
}

bool novastory::User::unsubscribe( const User& targetUser )
{
	if(!isLogined())
	{
		JSON_ERROR("not loggined", 1);
		return false;
	}

	int uid = userid();
	int targetid = targetUser.userid();
	if(targetid <= 0)
	{
		JSON_ERROR("target userid not founded", 2);
		return false;
	}

	SqlQuery query(QString("DELETE FROM subscriptions WHERE userid = '%1' AND targetid = '%2'").arg(uid).arg(targetid));
	return query.lastError().type() == QSqlError::NoError;
}

QList<int> novastory::User::subscriptions()
{
	int uid = userid();
	if(uid <= 0)
		return QList<int>();

	if(m_subscriptions_filled)
		return m_subscriptions;

	SqlQuery query(QString("SELECT * FROM subscriptions WHERE userid = '%1'").arg(uid));
	while(query.next())
	{
		m_subscriptions << query.value("targetid").toInt();
	}
	m_subscriptions_filled = true;
	return m_subscriptions;
}

QList<int> novastory::User::subscribed()
{
	int uid = userid();
	if(uid <= 0)
		return QList<int>();

	if(m_subscribed_filled)
		return m_subscribed;

	SqlQuery query(QString("SELECT * FROM subscriptions WHERE targetid = '%1'").arg(uid));
	while(query.next())
	{
		m_subscribed << query.value("userid").toInt();
	}
	m_subscribed_filled = true;
	return m_subscribed;
}

